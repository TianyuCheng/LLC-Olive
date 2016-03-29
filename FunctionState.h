#ifndef FUNCTIONSTATE_H
#define FUNCTIONSTATE_H

#include <string>
#include <fstream>
#include <iostream>

#include <map>
#include <set>
#include <vector>
#include <cassert>

#include "Value.h"
#include "RegisterAllocator.h"
#include "llc_olive.h"

class X86Inst;          // forward declaration
class X86Operand;
class TreeWrapper;
typedef struct tree* Tree;

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
    FunctionState(std::string name, int num_regs, int label = 0);
    virtual ~FunctionState();

    TreeWrapper* FindOrCreateLabel(llvm::BasicBlock *bb);
    void CreateVirtualReg(Tree t);
    void AssignVirtualReg(Tree lhs, Tree rhs);
    void CopyVirtualReg(VALUE &dst, VALUE &src);
    void GenerateLabelStmt(const char *label);
    void GenerateLabelStmt(VALUE &v);
    void GenerateMovStmt(X86Operand *dst, X86Operand *src);
    void GenerateBinaryStmt(const char *op, X86Operand *dst, X86Operand *src);

    std::string GetMCRegAt(int index) const { 
#if 0
        assert ((index >= 0 && index < virtual2machine.size()) && "virtual register index not in range");
        return std::string(registers[virtual2machine[index]]);
#else
        index = 0;
        return std::string(registers[virtual2machine[index]]);
#endif
    }
    void PrintAssembly(std::ostream &out);

    std::vector<LiveRange> GetRangeAnalysis() {
        assert(false && "get live range analysis not implemented yet");
        return std::vector<LiveRange>();        // NOT IMPLEMENTED
    }

    void CreateLocal(Tree t, int bytes = 8);
    X86Operand* GetLocalMemoryAddress(Tree t);

    void AddInst(X86Inst *inst) { assembly.push_back(inst); }
    void RestoreStack();

    // TreeMap management
    void AddToTreeMap(llvm::Instruction *instruction, TreeWrapper *t) {
        treeMap.insert(std::pair<llvm::Instruction*, TreeWrapper*>(instruction, t));
    }
    TreeWrapper* FindFromTreeMap(llvm::Instruction *instruction) const {
        auto it = treeMap.find(instruction);
        if (it != treeMap.end())
            return it->second;
        return nullptr;
    }

private:
    void LiveRangeAnalysis();
    std::string GetMachineReg(int virtual_reg);

private:
    // information about the function
    std::string function_name;
    int label;
    int local_bytes;

    // information about instruction and registers
    RegisterAllocator allocator;
    std::vector<int> virtual2machine;
    std::vector<X86Inst*> assembly;
    std::map<Tree, X86Operand*> locals;
    std::map<llvm::BasicBlock*, TreeWrapper*> labelMap;
    std::map<llvm::Instruction*, TreeWrapper*> treeMap;
};

typedef FunctionState* FUNCTION_STATE;


// =======================================================================

class X86Operand
{
    friend class X86Inst;
public:
    enum Type { X86Reg, X86Imm, X86Mem };

    X86Operand(FUNCTION_STATE fs, int r)
        : fstate(fs), type(Type::X86Reg), val(r), explicit_reg(true), base_address(nullptr), displacement(nullptr) {
    }

    X86Operand(FUNCTION_STATE fs, Type t, VALUE v) 
        : fstate(fs), type(t), val(v), base_address(nullptr), displacement(nullptr) {
        assert (t != Type::X86Mem && "X86Mem not support in X86Operand(Type t, VALUE) constructor.");
        // if operand is a register that is in some operand, then mark it as used but not allocated
    }

    X86Operand(FUNCTION_STATE fs, Type t, X86Operand *base, X86Operand *dis, int siz, int off) 
        : fstate(fs), type(t), val(0), base_address(base), displacement(dis), stride(siz), offset(off) {
        assert (t == Type::X86Mem && "X86Mem must be constructed using X86Operand(FUNCTION_STATE fs, Type t, X86Operand *base, X86Operand *dis, int siz, int off) constructor.");
    }

    virtual ~X86Operand() {
        if (base_address) { delete base_address; base_address = nullptr; }
        if (displacement) { delete displacement; displacement = nullptr; }
    }

    friend std::ostream& operator<<(std::ostream& out, X86Operand &op) {
        if (op.type == Type::X86Imm) {
            out << "$" << op.val;
        }
        else if (op.type == Type::X86Reg) {
            if (op.explicit_reg) {
                out << "%" << registers[op.val.AsVirtualReg()];
            }
            else
#if 0
                out << "$" << op.fstate->GetMCRegAt(op.val.AsVirtualReg());
#else
                out << "%" << op.val.AsVirtualReg();
#endif
        }
        else {
            // output memory address by x86 addressing mode
            if (op.offset)          out << op.offset;
            out << "(";
            if (op.base_address)    out << *op.base_address;
            else                    out << "0";
            if (op.stride && op.displacement)    out << ", " << *op.displacement << ", " << op.stride;
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
    bool explicit_reg;      // not specified by function

    // for mem
    X86Operand *base_address;
    X86Operand *displacement;
    int stride;
    int offset;
};

class X86Inst
{
public:
    X86Inst(const char *op)
        : opname(op), dst(nullptr), src(nullptr) {
        // single operand instruction
    }
    X86Inst(const char *op, X86Operand *op1) 
        : opname(op), dst(op1), src(nullptr) {
        // single operand instruction
    }
    X86Inst(const char *op, X86Operand *op1, X86Operand *op2)
        : opname(op), dst(op1), src(op2) {
        // double operand instruction
    }
    virtual ~X86Inst() {
#if 1   // DO NOT FREE X86Mem HERE, IT WILL CAUSE DOUBLE FREE for function locals
        if (dst && dst->type != X86Operand::Type::X86Mem) { delete dst; dst = nullptr; }
        if (src && src->type != X86Operand::Type::X86Mem) { delete src; src = nullptr; }
#endif
    }

    friend std::ostream& operator<<(std::ostream& out, X86Inst &inst) {
        out << "\t" << inst.opname;
        if (inst.dst) out << "\t" << *(inst.dst);
        if (inst.src) out << ", " << *(inst.src);
        out << std::endl;
        return out;
    }
private:
    std::string opname;
    X86Operand *dst;
    X86Operand *src;
};

class X86Label : public X86Inst
{
public:
    X86Label(const char *label) : X86Inst(label) {
    }
    virtual ~X86Label() {
    }

    friend std::ostream& operator<<(std::ostream& out, X86Label &inst) {
        out << inst.label << ":" << std::endl;
        return out;
    }
private:
    std::string label;
};


typedef X86Operand::Type OP_TYPE;

// =======================================================================

#endif /* end of include guard: FUNCTIONSTATE_H */
