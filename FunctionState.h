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
#include "GlobalState.h"

class X86Inst;          // forward declaration
class X86Operand;
class Tree;
class RegisterAllocator;

#define PRINT_TO_PHYSICAL_REG    1

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

    void GenerateStmt(const char *op);
    void GenerateUnaryStmt(const char *op, Tree *src, const char* suffix=nullptr);
    void GenerateBinaryStmt(const char *op, Tree *dst, Tree *src, const char* suffix=nullptr);
    void GenerateBinaryStmt(const char *op, X86Operand *dst, X86Operand *src, const char *suffix=nullptr);

    void GenerateMovStmt(Tree *dst, Tree *src, const char* suffix="q");
    void GenerateMovStmt(X86Operand *dst, X86Operand *src, const char* suffix="q");
    void GenerateLabelStmt(const char *label);
    void GenerateLabelStmt(Tree *v);
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
    void AddToPhiMap(llvm::PHINode *phi) {
        Tree *phiTree = new Tree(REG);
        CreateVirtualReg(phiTree);
        phiRegs.insert(std::pair<llvm::PHINode*, Tree*>(phi, phiTree));
        phiTree->UseAsVirtualRegister();

        for (auto it = phi->block_begin(); it != phi->block_end(); it++) {
            llvm::BasicBlock *bb = *it;
            llvm::Value *v = phi->getIncomingValueForBlock(bb);
            auto key = std::pair<llvm::PHINode*, llvm::BasicBlock*>(phi, bb);
            phiMap[key] = v;
        }
    }
    void BasicBlockProcessPhi(GlobalState &gstate, std::vector<Tree*> &treeList, llvm::BasicBlock *bb) {
        for (auto it = phiMap.begin(); it != phiMap.end(); it++) {
            auto key = it->first;
            if (key.second != bb) continue;        // find corresponding pairs for this basic block
            // llvm::errs() << "-------------------------- PHI --------------------------\n";
            auto phi = key.first;
            auto value = it->second;
            Tree *phi_reg = phiRegs[phi];

            using namespace llvm;
            // errs() << "FOUND PHI OPERAND: ";
            // value->print(errs()); errs() << "\n";

            Tree *t = new Tree(PHI);
            t->SetValue(phi_reg->GetValue());
            t->UseAsPhi();
            t->SetParent(phi_reg);
            t->GetTreeRef();
            // llvm::errs() << "PHI-REG VALUE: " << phi_reg->GetValue().val.i32s << "\n";

            if (Instruction *inst = dyn_cast<Instruction>(value)) {
                Tree *child = FindFromTreeMap(inst);
                assert(child && "phi operand (instruction as reg) must be previously computed");
                t->AddChild(child->GetTreeRef());
            }
            else if (Argument *arg = dyn_cast<Argument>(value)) {
                Tree *child = FindFromTreeMap(arg);
                assert(child && "phi operand (argument) must be previously computed");
                t->AddChild(child->GetTreeRef());
            }
            else if (Constant *def = dyn_cast<Constant>(value)) {
                if (ConstantInt *cnst = dyn_cast<ConstantInt>(value)) {
                    // check if the operand is a constant int
                    t->CastInt(cnst);
                }
                else if (ConstantFP *cnst = dyn_cast<ConstantFP>(value)) {
                    // check if the operand is a constant int
                    t->CastFP(cnst);
                }
                else if (GlobalVariable *gv = dyn_cast<GlobalVariable>(value)) {
                    Tree *wrapper = gstate.FindFromGlobalMap(gv);
                    assert(wrapper && "global operands must be previously defined");
                    t->AddChild(wrapper->GetTreeRef());      // automatically increase the refcnt
                }
                // ... There are many kinds of constant, right now we do not deal with them ...
                else {
                    // this is bad and probably needs to terminate the execution
                    errs() << "NOT IMPLEMENTED OTHER CONST TYPES FOR PHI:\t"; value->print(errs()); errs() << "\n";
                    exit(EXIT_FAILURE);
                }
            }
            else {
                using namespace llvm;
                value->print(errs()); errs() << "\n";
                assert(false && "unsupported phi operands");
            }
            treeList.push_back(t);
            // llvm::errs() << "PHI-REG refcnt: " << phi_reg->GetRefCount() << "\n";
            // RecordLiveStart(phi_reg->GetTreeRef());
            // llvm::errs() << "------------------------ PHI END ------------------------\n";
        }
    }

    Tree* FindFromTreeMap(llvm::Instruction *instruction) {
        auto it = instMap.find(instruction);
        if (it != instMap.end())
            return it->second;

        if (llvm::PHINode::classof(instruction)) {
            llvm::PHINode *node = llvm::dyn_cast<llvm::PHINode>(instruction);
            assert(node && "cast to phi node must be successful");
            return phiRegs[node];
        }
        return nullptr;
    }
    Tree* FindFromTreeMap(llvm::Argument *arg) {
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
    int GetNumArgs() const { return num_args; }
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
    std::map<std::pair<llvm::PHINode*, llvm::BasicBlock*>, llvm::Value*> phiMap;
    std::map<llvm::PHINode*, Tree*> phiRegs;
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

    X86Operand(FUNCTION_STATE fs, OP_TYPE t, std::string v, bool unused) 
        : fstate(fs), type(t), val(0), explicit_reg(false), base_address(nullptr), displacement(nullptr) {
        assert (t == OP_TYPE::X86Function || t == OP_TYPE::X86GlobalValue );
        name = v;
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
#if PRINT_TO_PHYSICAL_REG
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
        else if (op.type == OP_TYPE::X86Function || op.type == OP_TYPE::X86GlobalValue) {
            out << op.name;
        }
        else {
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
    std::string name;

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
#if PRINT_TO_PHYSICAL_REG
        if (dst) dst->ResolveRegs(fstate, out);
        if (src) src->ResolveRegs(fstate, out);
#endif
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
    bool IsCallBegin() const { return opname == std::string("call-begin"); }    // before register push
    bool IsCallEnd() const { return opname == std::string("call-end"); }        // after register pop
private:
    std::string opname;
    X86Operand *dst;
    X86Operand *src;
    bool isLabel;
};

// =======================================================================

#endif /* end of include guard: FUNCTIONSTATE_H */
