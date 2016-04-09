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
#include "llc_olive.h"
#include "LiveRange.h"
#include "RegisterAllocator.h"

class X86Inst;          // forward declaration
class X86Operand;
class Tree;
class RegisterAllocator;

/**
 * Function State Keeper
 * */
class FunctionState
{
public:
    FunctionState(std::string name, int num_regs, int function_id = 0, int label_id = 0);
    virtual ~FunctionState();

    Tree* FindLabel(llvm::BasicBlock *bb);
    Tree* CreateLabel(llvm::BasicBlock *bb);
    void CreateLocal(Tree *t, int bytes = 8);
    X86Operand* GetLocalMemoryAddress(Tree *t);
    X86Operand* GetLocalMemoryAddress(int offset);
    void CreateArgument(llvm::Argument *arg);
    void CreateVirtualReg(Tree *t);
    void CreatePhysicalReg(Tree *t, Register r);
    void AssignVirtualReg(Tree *lhs, Tree *rhs);
    void LoadFromReg(Tree *dst, Tree *src);
    void LoadFromImm(Tree *dst, Tree *src);
    void LoadEffectiveAddress(Tree *dst, Tree *src);
    void GenerateLabelStmt(const char *label);
    void GenerateLabelStmt(Tree *v);
    void GenerateMovStmt(Tree *dst, Tree *src);
    void GenerateMovStmt(X86Operand *dst, X86Operand *src);
    void GenerateStmt(const char *op);
    void GenerateUnaryStmt(const char *op, Tree *src, bool suffix=true);
    void GenerateBinaryStmt(const char *op, Tree *dst, Tree *src, bool suffix=true);
    void GenerateBinaryStmt(const char *op, X86Operand *dst, X86Operand *src, bool suffix=true);
    void GeneratePushStmt(Tree *t);
    void GeneratePushStmt(Register r);
    void GeneratePopStmt(Tree *t);
    void GeneratePopStmt(Register r);

    // register allocation
    int  CreateSpill(std::ostream &out, int reg_idx);
    void RestoreSpill(std::ostream &out, int reg_idx, int offset);

    std::string GetFuncName() const { return function_name; }
    std::string GetMCRegAt(int index) { 
        return allocator.GetMCRegAt(index);
    }
    void PrintAssembly(std::ostream &out/*, RegisterAllocator &ra*/);

    std::vector<LiveRange> GetRangeAnalysis() {
        assert(false && "get live range analysis not implemented yet");
        return std::vector<LiveRange>();        // NOT IMPLEMENTED
    }

    void AddInst(X86Inst *inst) { assembly.push_back(inst); }
    // void RestoreStack();

    // TreeMap management
    void AddToTreeMap(llvm::Instruction *instruction, Tree *t) {
        instMap.insert(std::pair<llvm::Instruction*, Tree*>(instruction, t));
    }
    void AddToTreeMap(llvm::Argument *arg, Tree *t) {
        argsMap.insert(std::pair<llvm::Argument*, Tree*>(arg, t));
    }
    Tree* FindFromTreeMap(llvm::Instruction *instruction) const {
        auto it = instMap.find(instruction);
        if (it != instMap.end())
            return it->second;
        return nullptr;
    }
    Tree* FindFromTreeMap(llvm::Argument *arg) const {
        auto it = argsMap.find(arg);
        if (it != argsMap.end())
            return it->second;
        return nullptr;
    }

    void RecordLiveness(Tree *t);
    void RecordLiveStart(Tree *t);
    void Allocate(VALUE &v, std::ostream &out) {
        allocator.Allocate(this, out, v.AsVirtualReg(), current_line);
    }

    bool RegisterInUse(Register r) { return allocator.RegisterInUse(r); }

    int GetLabelID() const { return label_id; }
    int GetFunctionID() const { return function_id; }
private:
    // information about the function
    std::string function_name;
    int label_id;
    int function_id;

    int local_bytes;
    int num_args;
    int current_line;

    // information about instruction and registers
    SimpleRegisterAllocator allocator;
    std::vector<X86Inst*> assembly;

    std::map<Tree*, X86Operand*> locals;
    std::map<llvm::BasicBlock*, Tree*> labelMap;
    std::map<llvm::Instruction*, Tree*> instMap;
    std::map<llvm::Argument*, Tree*> argsMap;
    std::vector<X86Operand*> freeList;
};

typedef FunctionState* FUNCTION_STATE;


// =======================================================================

class X86Operand
{
    friend class X86Inst;
public:

    X86Operand(FUNCTION_STATE fs, int r)
        : fstate(fs), type(OP_TYPE::X86Reg), val(r), explicit_reg(true), base_address(nullptr), displacement(nullptr) {
            // std::cerr << "CREATE A PHYSICAL REGISTER: " << registers[r] << std::endl;
    }

    X86Operand(FUNCTION_STATE fs, OP_TYPE t, VALUE v) 
        : fstate(fs), type(t), val(v), explicit_reg(false), base_address(nullptr), displacement(nullptr) {
        assert (t == OP_TYPE::X86Reg || t == OP_TYPE::X86Imm || t == OP_TYPE::X86Label );
        // if operand is a register that is in some operand, then mark it as used but not allocated
    }

    X86Operand(FUNCTION_STATE fs, OP_TYPE t, X86Operand *base, X86Operand *dis, int siz, int off) 
        : fstate(fs), type(t), val(0), explicit_reg(false), base_address(base), displacement(dis), stride(siz), offset(off) {
        assert (t == OP_TYPE::X86Mem && "X86Mem must be constructed using X86Operand(FUNCTION_STATE fs, OP_TYPE t, X86Operand *base, X86Operand *dis, int siz, int off) constructor.");
    }

    virtual ~X86Operand() {
        // if (base_address) { delete base_address; base_address = nullptr; }
        // if (displacement) { delete displacement; displacement = nullptr; }
    }

    friend std::ostream& operator<<(std::ostream& out, X86Operand &op) {
        if (op.type == OP_TYPE::X86Imm) {
            out << "$" << op.val;
        }
        else if (op.type == OP_TYPE::X86Reg) {
            if (op.explicit_reg) {
                out << "%" << registers[op.val.AsVirtualReg()];
            }
            else {
#if 1
                out << "%" << op.fstate->GetMCRegAt(op.val.AsVirtualReg());
#else
                out << "%" << op.val.AsVirtualReg();
#endif
            }
        }
        else if (op.type == OP_TYPE::X86Mem) {
            // output memory address by x86 addressing mode
            if (op.offset)          out << op.offset;
            out << "(";
            if (op.base_address)    out << *op.base_address;
            else                    out << "0";
            if (op.stride && op.displacement) {
                if (op.displacement->GetType() == OP_TYPE::X86Reg)
                    out << ", " << *op.displacement << ", " << op.stride;
            }
            out << ")";
        }
        else if (op.type == OP_TYPE::X86Label) {
            out << op.val.AsLabel();
        }
        else if (op.type == OP_TYPE::X86Function) {
            out << op.func_name;
        } else {
            assert(false && "OP_TYPE is invalid!");
        }
        return out;
    }

    OP_TYPE GetType() const { return type; }
    bool IsVirtualReg() const { return type == OP_TYPE::X86Reg && !explicit_reg; }
    int GetVirtualReg() const { assert(IsVirtualReg()); return val.AsVirtualReg(); }

    int GetOffset() const { return offset; }

    void ResolveRegs(FunctionState *fstate, std::ostream &out) {
        switch (type) {
            case OP_TYPE::X86Reg:
                if (!explicit_reg) fstate->Allocate(val, out);
                break;
            case OP_TYPE::X86Mem:
                if (base_address) base_address->ResolveRegs(fstate, out);
                if (displacement) displacement->ResolveRegs(fstate, out);
                break;
            default:
                break;
        }
    }

private:
    FUNCTION_STATE fstate;
    // operand type differentor
    OP_TYPE type;
    std::string func_name;

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
    X86Inst(const char *op, bool isL = false)
        : opname(op), dst(nullptr), src(nullptr), isLabel(isL) {
        // single operand instruction
    }
    X86Inst(const char *op, X86Operand *op1) 
        : opname(op), dst(op1), src(nullptr) {
        // single operand instruction
        isLabel = false;
    }
    X86Inst(const char *op, X86Operand *op1, X86Operand *op2)
        : opname(op), dst(op1), src(op2) {
        // double operand instruction
        isLabel = false;
    }
    virtual ~X86Inst() {
    }

    X86Operand *GetDst() const { return dst; }
    X86Operand *GetSrc() const { return src; }

    void ResolveRegs(FunctionState *fstate, std::ostream &out) {
        if (dst) dst->ResolveRegs(fstate, out);
        if (src) src->ResolveRegs(fstate, out);
    }

    friend std::ostream& operator<<(std::ostream& out, X86Inst &inst) {
        if (!inst.isLabel) {
            out << "\t" << inst.opname;
            if (inst.src) out << "\t" << *(inst.src) << ",";
            if (inst.dst) out << "\t" << *(inst.dst);
        } else {
            out << inst.opname << ":";
        }
        return out;
    }

    bool IsCall() const { 
        return opname.compare(0, 4, "call") == 0;
    }
private:
    std::string opname;
    X86Operand *dst;
    X86Operand *src;
    bool isLabel;
};

// =======================================================================

#endif /* end of include guard: FUNCTIONSTATE_H */
