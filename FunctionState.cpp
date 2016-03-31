#include "FunctionState.h"

FunctionState::FunctionState(std::string name, int n, int l)
    : function_name(name), label(l), local_bytes(8), allocator(n)
{
    // initialize function state here
    // local_bytes is initiated to 8 for saved rip
}

FunctionState::~FunctionState() {
    // de-initialize function state here
    for (X86Inst *inst : assembly)
        delete inst;
    for (auto it = labelMap.begin(); it != labelMap.end(); ++it)
        delete it->second;
    for (auto it = liveness.begin(); it != liveness.end(); ++it)
        delete it->second;
}

void FunctionState::PrintAssembly(std::ostream &out) {
    out << "#####################################################" << std::endl;
    this->PrintLiveness(out);
    out << "#####################################################" << std::endl;
    // print assembly to file
    
    // pass liveness analysis to register allocator
    allocator.set_intervals(liveness);
    allocator.linearScanAllocate();
    virtual2machine = allocator.get_virtual2machine();

    // print function entrance
    // TODO: make these a part of the assembly code
    if (std::string(function_name) == std::string("main"))
        out << "\t.globl" << std::endl;

    out << "main:" << std::endl;
    out << "\tpushq\t%rbp" << std::endl;
    out << "\tsubq\t%rsp, $" << local_bytes << std::endl;

    // TODO: remember to print function begin and ends
    for (X86Inst *inst : assembly)
        out << *inst;
    for(auto it = locals.begin(); it != locals.end(); ++it ) {
        delete it->second;
    }
}

Tree* FunctionState::FindLabel(llvm::BasicBlock *bb) {
    auto it = labelMap.find(bb);
    if (it != labelMap.end())
        // find already mapped label
        return it->second;
    return nullptr;
}

Tree* FunctionState::CreateLabel(llvm::BasicBlock *bb) {
    auto it = labelMap.find(bb);
    if (it != labelMap.end())
        return it->second;

    // this basic block has never been seen,
    // assign a new label, add it to the label map
    Tree *treeLabel = new Tree(LABEL);
    treeLabel->SetValue(label++);
    labelMap.insert(std::pair<llvm::BasicBlock*, Tree*>(bb, treeLabel));
    return treeLabel;
}

void FunctionState::CreateLocal(Tree *t, int bytes) {
    // already allocated
    auto it = locals.find(t);
    if (it != locals.end())
        return;

    // now allocate local variable
    if (bytes % 4 != 0) bytes += 4 - bytes % 4;
    local_bytes += bytes;
    X86Operand *local = new X86Operand(this, OP_TYPE::X86Mem, 
            new X86Operand(this, RBP),                     // base_address, should be $rbp
            new X86Operand(this, OP_TYPE::X86Imm, 0),      // displacement
            0,                                             // multiplier    
            local_bytes - bytes);
    // save the local variable memory address for future use
    locals.insert(std::pair<Tree*, X86Operand*>(t, local));
}

X86Operand* FunctionState::GetLocalMemoryAddress(Tree *t) {
    auto it = locals.find(t);
    assert (it != locals.end() && "GetLocalMemoryAddress cannot find associated address for input tree");
    return it->second;
}

void FunctionState::RestoreStack() {
    if (local_bytes > 0)
        assembly.push_back(new X86Inst("addq",
                new X86Operand(this, RSP),   // should be $rsp
                new X86Operand(this, OP_TYPE::X86Imm, local_bytes)
        ));
}

void FunctionState::CreateVirtualReg(Tree *t) {
    int v = virtual2machine.size();
    virtual2machine.push_back(-1);      // -1 not allocated
    t->val = v;
    t->UseAsRegister();
    RecordLiveStart(t);                 // newly allocated virtual register must be added to liveness
}

void FunctionState::AssignVirtualReg(Tree *lhs, Tree *rhs) {
    lhs->UseAsRegister();
    if (rhs->GetRefCount() == 1) {
        // this register is about to be free, we can reuse it
        lhs->val = rhs->val;
    }
    else {
        // we will still be using rhs in the future, 
        // so better not overwrite the rhs
        CreateVirtualReg(lhs);      // assign a virtual register to the inst
        LoadFromReg(lhs->val, rhs->val);
    }
}

void FunctionState::LoadFromReg(VALUE &dst, VALUE &src) {
    if (src.val.i32s == dst.val.i32s) return;
    // only copy register when src and dst are different
    GenerateMovStmt(
        new X86Operand(this, OP_TYPE::X86Reg, dst),
        new X86Operand(this, OP_TYPE::X86Reg, src)
    );
}

void FunctionState::LoadFromImm(VALUE &dst, VALUE &src) {
    GenerateMovStmt(
        new X86Operand(this, OP_TYPE::X86Reg, dst),
        new X86Operand(this, OP_TYPE::X86Imm, src)
    );
}

void FunctionState::GenerateLabelStmt(const char *l) {
    AddInst(new X86Inst(l, true));
}

void FunctionState::GenerateLabelStmt(VALUE &v) {
    AddInst(new X86Inst(v.AsLabel(), true));
}

void FunctionState::GenerateMovStmt(X86Operand *dst, X86Operand *src) {
    // Keep this one-line function, since we might want 
    // to migrate to different operand sizes, so we will
    // be using movb, movw, movl, movq according to the
    // operands
    GenerateBinaryStmt("mov", dst, src);
}

void FunctionState::GenerateBinaryStmt(const char *op_raw, X86Operand *dst, X86Operand *src) {
    // Keep this one-line function, since we might want 
    // to migrate to different operand sizes, so we will
    // be using suffixes b, w, l, q according to the
    // operands
    std::string op = std::string(op_raw) + "q";
    AddInst(new X86Inst(op.c_str(), dst, src));
}


void FunctionState::RecordLiveness(Tree *t) {
    t->RemoveRef();                // decrease the reference counter

    if (!t->IsVirtualReg()) return;      // we only care about registers' references

    int reg = t->val.AsVirtualReg();
    if (t->GetRefCount() == 0) {
        // this register is dead now, we should set the endLine
        auto it = liveness.find(reg);
        assert(it != liveness.end() && "a dead register should have already in the liveness analysis");

        LiveRange *range = it->second;
        int endLine = assembly.size() - 1;    // ending at current size
        range->endpoint = endLine;
        liveness.insert(std::pair<int, LiveRange*>(reg, range));
    }
    // else this register is still live now, do not do anything
}

void FunctionState::PrintLiveness(std::ostream &out) {
    for (auto it = liveness.begin(); it != liveness.end(); ++it) {
        LiveRange *range = it->second;
        out << "Virtual Register " << it->first << ":\t" 
            << "start:\t" << range->startpoint << "\t"
            << "end:\t"   << range->endpoint << std::endl;
    }
}

void FunctionState::RecordLiveStart(Tree *t) {
    if (t->IsVirtualReg() && t->GetRefCount() > 0) {
        int reg = t->val.AsVirtualReg();
        auto it = liveness.find(reg);
        if (it == liveness.end()) {
            // not found in liveness analysis, insert it
            // this register's life starts from now
            // we do not know when it ends yet
            int startLine = assembly.size();
            liveness.insert(std::pair<int, LiveRange*>(reg, new LiveRange(startLine)));
        }
        // if found, then it means this virtual register may be assigned from an about-to-die register
        // in this case, we do not need to do anything in particular
    }
    // this register is dead if refcnt == 0
}
