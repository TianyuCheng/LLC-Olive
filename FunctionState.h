#ifndef FUNCTIONSTATE_H
#define FUNCTIONSTATE_H

#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <cassert>

#include "Value.h"
#include "RegisterAllocator.h"

class X86Inst;          // forward declaration

struct LiveRange {
    int begin;
    int end;
    LiveRange(int start, int stop) {
        begin = start;
        end = stop;
    }
};

/**
 * Function State Keeper
 * */
class FunctionState
{
public:
    FunctionState(std::string name, int num_regs, int loop_label = 0);
    virtual ~FunctionState();

    int GetVirtualReg() {
        int v = virtual2machine.size();
        virtual2machine.push_back(-2);      // -2 not used yet
                                            // -1 not allocated yet
        return v;
    }
    std::string GetMCRegAt(int index) const { 
        assert ((index < 0 || index > virtual2machine.size()) && "virtual register index not in range");
        return std::string(registers[virtual2machine[index]]);
    }
    void PrintAssembly(std::ostream &out);

    std::vector<LiveRange> GetRangeAnalysis() {
        assert(false && "get live range analysis not implemented yet");
        return std::vector<LiveRange>();        // NOT IMPLEMENTED
    }

private:
    void LiveRangeAnalysis();
    std::string GetMachineReg(int virtual_reg);

private:
    // information about the function
    std::string function_name;
    int loop_label;
    int num_locals;

    // information about instruction and registers
    std::vector<int> virtual2machine;
    std::vector<X86Inst> assembly;
    RegisterAllocator allocator;
};

typedef FunctionState* FUNCTION_STATE;


// =======================================================================

class X86Operand
{
    friend class X86Inst;
public:
    enum Type { X86Reg, X86Imm, X86Mem };

    X86Operand(FUNCTION_STATE fs, Type t, VALUE v) 
        : fstate(fs), type(t), val(v), base_address(nullptr), displacement(nullptr) {
        assert (t != Type::X86Mem && "X86Mem not support in X86Operand(Type t, VALUE) construction.");
    }

    X86Operand(FUNCTION_STATE fs, Type t, X86Operand *base, X86Operand *dis, int siz, int off) 
        : fstate(fs), type(t), val(0), base_address(base), displacement(dis), stride(siz), offset(off) {
        assert (t == Type::X86Mem && "X86Mem must be constructed using X86Operand(Type t, VALUE) construction.");
    }

    virtual ~X86Operand() {
#if 0
        if (base_address) { delete base_address; base_address = nullptr; }
        if (displacement) { delete displacement; displacement = nullptr; }
#endif
    }

    friend std::ostream& operator<<(std::ostream& out, X86Operand &op) {
        if (op.type == Type::X86Imm) {
            out << "$" << op.val;
        }
        else if (op.type == Type::X86Reg) {
            out << "$" << op.fstate->GetMCRegAt(op.val.AsVirtualReg());
        }
        else {
            // output memory address by x86 addressing mode
            if (op.offset)          out << std::hex << op.offset;
            out << "(";
            if (op.base_address)    out << op.base_address;
            else                    out << "0";
            if (op.displacement)    out << ", " << op.displacement << ", " << op.stride;
            out << ")";
        }
        return out;
    }

private:
    FUNCTION_STATE fstate;
    // operand type differentor
    Type type;

    // for imm and reg
    VALUE val;

    // for mem
    X86Operand *base_address;
    X86Operand *displacement;
    int stride;
    int offset;
};

class X86Inst
{
public:
    X86Inst(const char *op, X86Operand op1, X86Operand op2) 
        : opname(op), dst(op1), src(op2) {
    }

    friend std::ostream& operator<<(std::ostream& out, X86Inst &inst) {
        out << "\t" << inst.opname 
            << "\t" << inst.dst << ", " 
            << inst.src << std::endl;
        return out;
    }
private:
    std::string opname;
    X86Operand dst;
    X86Operand src;
};

// =======================================================================

#endif /* end of include guard: FUNCTIONSTATE_H */
