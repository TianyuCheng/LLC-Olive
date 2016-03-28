#include "FunctionState.h"

FunctionState::FunctionState(std::string name, int n, int l)
    : function_name(name), loop_label(l), local_bytes(0), allocator(n)
{
    // initialize function state here
}

FunctionState::~FunctionState() {
    // de-initialize function state here
    for (X86Inst *inst : assembly)
        delete inst;
}

std::string FunctionState::GetMachineReg(int virtual_reg) {
    // allocate a free register
    return allocator.Allocate();
}

void FunctionState::PrintAssembly(std::ostream &out) {
    this->LiveRangeAnalysis();
    // print assembly to file
    
    // print function entrance
    // TODO: make these a part of the assembly code
    if (std::string(function_name) == std::string("main"))
        out << "main:" << std::endl;
    else
        out << "." << this->function_name << ":" << std::endl;

    out << "\tpushq\t$rbp" << std::endl;
    out << "\tsubq\t$rsp, $" << local_bytes << std::endl;

    // TODO: remember to print function begin and ends
    for (X86Inst *inst : assembly)
        out << *inst;
    for(auto it = locals.begin(); it != locals.end(); ++it ) {
        delete it->second;
    }
}

void FunctionState::LiveRangeAnalysis() {
    // analyze live range of virtual registers
}

void FunctionState::CreateLocal(Tree t, int bytes) {
    // already allocated
    auto it = locals.find(t);
    if (it != locals.end())
        return;

    // now allocate local variable
    if (bytes % 4 != 0) bytes += 4 - bytes % 4;
    local_bytes += bytes;
    X86Operand *local = new X86Operand(this, OP_TYPE::X86Mem, 
            new X86Operand(this, OP_TYPE::X86Reg, RBP),   // base_address, should be $rbp
            new X86Operand(this, OP_TYPE::X86Imm, 0),      // displacement
            0,                                             // multiplier    
            local_bytes - bytes);
    // save the local variable memory address for future use
    locals.insert(std::pair<Tree, X86Operand*>(t, local));
}

X86Operand* FunctionState::GetLocalMemoryAddress(Tree t) {
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

void FunctionState::CreateVirtualReg(Tree t) {
    int v = virtual2machine.size();
    virtual2machine.push_back(-1);      // -1 not allocated
    t->val = v;
    t->isReg = true;
}

void FunctionState::AssignVirtualReg(Tree lhs, Tree rhs) {
    rhs->refcnt--;                           // discharge 1 reference
    lhs->isReg = true;
    if (rhs->refcnt == 0) {
        // this register is free now, we can reuse it
        lhs->val = rhs->val;
    }
    else {
        // we will still be using rhs in the future, 
        // so better not overwrite the rhs
        CreateVirtualReg(lhs);      // assign a virtual register to the inst
        CopyVirtualReg(lhs->val, rhs->val);
    }
}

void FunctionState::CopyVirtualReg(VALUE &dst, VALUE &src) {
    if (src.val.i32s == dst.val.i32s) return;
    // only copy register when src and dst are different
    GenerateMovStmt(
        new X86Operand(this, OP_TYPE::X86Reg, dst),
        new X86Operand(this, OP_TYPE::X86Reg, src)
    );
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
