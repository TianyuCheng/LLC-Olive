#include "FunctionState.h"

FunctionState::FunctionState(std::string name, int n, int l)
    : function_name(name), label(l), local_bytes(8), num_args(0), allocator(n), current_line(0)
{
    // initialize function state here
    // local_bytes is initiated to 8 for saved rip
#if 0
    // preserve callee saved registers
    for (int i = 0; i < 7; i++)
        GeneratePushStmt(callee_saved_regs[i]);
    local_bytes = 7 * 8;    // space for 8 callee_saved registers
#endif
}

FunctionState::~FunctionState() {
    // de-initialize function state here
    for (X86Inst *inst : assembly)
        delete inst;
    for (auto it = labelMap.begin(); it != labelMap.end(); ++it)
        delete it->second;
    for (auto operand : freeList)
        delete operand;
}

void FunctionState::PrintAssembly(std::ostream &out/*, RegisterAllocator &ra*/) {
#if DEBUG
    allocator.PrintLiveness(out);
#endif
    // print assembly to file
    
    // pass liveness analysis to register allocator
    /*
       allocator.set_intervals(liveness);
    allocator.linearScanAllocate();
    */
    // virtual2machine = ra.get_virtual2machine();

    // print function entrance
    out << "\t.globl " << function_name << std::endl << std::endl;
    out << "\t.type " << function_name  << ", @function"<< std::endl;

    // prolog
    out << function_name  << ":" << std::endl;
    out << "\tpushq\t%rbp" << std::endl;
    out << "\tmovq\t%rsp,\t%rbp" << std::endl;
    // out << "\tsubq\t%rsp, $" << local_bytes << std::endl;

    int lineNo = 0;

    // TODO: remember to print function begin and ends
    for (X86Inst *inst : assembly) {
        inst->ResolveRegs(this, out);
        out << *inst;
#if DEBUG
        out << "\t; line " << lineNo++;
#endif
        out << std::endl;
        current_line++;
    }

    // epilog
    std::stringstream ss;
    ss << "." << function_name << "End";
    std::string s = ss.str();
    GenerateLabelStmt(s.c_str());
#if 0
    // restore registers
    for (int i = 6; i >= 0; i--)
        GeneratePopStmt(callee_saved_regs[i]);
#endif
    out << "\tmovq\t%rbp,\t%rsp" << std::endl;
    out << "\tleave" << std::endl;
    out << "\tret" << std::endl;

    for(auto it = locals.begin(); it != locals.end(); ++it ) {
        delete it->second;
    }
    current_line = 0;
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
    if (it != locals.end()) return;

    // now allocate local variable
    if (bytes % 4 != 0) bytes += 4 - bytes % 4;
    local_bytes += bytes;
    X86Operand *local = new X86Operand(this, OP_TYPE::X86Mem, 
            new X86Operand(this, RBP),                     // base_address, should be rbp
            new X86Operand(this, OP_TYPE::X86Imm, 0),      // displacement
            0,                                             // multiplier    
            -(local_bytes - bytes));                       // negate because locals are below rbp
    // save the local variable memory address for future use
    locals.insert(std::pair<Tree*, X86Operand*>(t, local));
}

X86Operand* FunctionState::GetLocalMemoryAddress(Tree *t) {
    auto it = locals.find(t);
    assert (it != locals.end() && "GetLocalMemoryAddress cannot find associated address for input tree");
    return it->second;
}

X86Operand* FunctionState::GetLocalMemoryAddress(int offset) {
    X86Operand *local = new X86Operand(this, OP_TYPE::X86Mem, 
            new X86Operand(this, RBP),                     // base_address, should be rbp
            nullptr,
            0,                                             // multiplier    
            offset);
    return local;
}

// void FunctionState::RestoreStack() {
//     if (local_bytes > 0)
//         assembly.push_back(new X86Inst("addq",
//                 new X86Operand(this, RSP),   // should be rsp
//                 new X86Operand(this, OP_TYPE::X86Imm, local_bytes)
//         ));
// }

void FunctionState::CreateArgument(llvm::Argument *arg) {
    Tree *t = nullptr;
    X86Operand *op = nullptr;

    if (num_args < 6) {                 // first six parameters are passed in registers
        t = new Tree(REG, params_regs[num_args]);
        t->SetLLVMValue(arg);
        CreatePhysicalReg(t, params_regs[num_args]);
        // insert into liveness: should be live from start
        int startLine = assembly.size();
        allocator.RecordLiveStart(t->GetValue().AsVirtualReg(), startLine);
    } else {
        t = new Tree(MEM);
        t->SetLLVMValue(arg);
        op = new X86Operand(this, OP_TYPE::X86Mem, 
            new X86Operand(this, RBP),                     // base_address, should be rbp
            new X86Operand(this, OP_TYPE::X86Imm, 0),      // displacement
            0,                                             // multiplier    
            8 * (num_args - 6 + 1));                       // offset
        // std::cerr << "Arg offset: " << 8 * (num_args - 6 + 1) << std::endl;
        locals.insert(std::pair<Tree*, X86Operand*>(t, op));
    }
    argsMap.insert(std::pair<llvm::Argument*, Tree*>(arg, t));
    num_args++;
}

void FunctionState::CreateVirtualReg(Tree *t) {
    using namespace llvm;
    int v = allocator.CreateVirtualReg();
    // llvm::Value *val = t->GetLLVMValue();
    // assert(val && "llvm value should not be null");
    t->val = v;
    t->UseAsVirtualRegister();
    RecordLiveStart(t);                 // newly allocated virtual register must be added to liveness
}

void FunctionState::CreatePhysicalReg(Tree *t, Register r) {
    t->val = r;
    t->UseAsPhysicalRegister();
    RecordLiveStart(t);                 // newly allocated virtual register must be added to liveness
}

void FunctionState::AssignVirtualReg(Tree *lhs, Tree *rhs) {
    lhs->UseAsVirtualRegister();
    if (rhs->GetRefCount() == 1) {
        // this register is about to be free, we can reuse it
        lhs->val = rhs->val;
        lhs->UseAsVirtualRegister();
    }
    else {
        // we will still be using rhs in the future, 
        // so better not overwrite the rhs
        CreateVirtualReg(lhs);      // assign a virtual register to the inst
        LoadFromReg(lhs, rhs);
    }
}

void FunctionState::LoadFromReg(Tree *dst, Tree *src) {
    // if (src->IsVirtualReg() && dst->IsVirtualReg())
    //     if (src->GetValue().AsVirtualReg() != dst->GetValue().AsVirtualReg())
    //         return;
    // if (src->IsPhysicalReg() && dst->IsPhysicalReg())
    //     if (src->GetValue().AsVirtualReg() != dst->GetValue().AsVirtualReg())
    //         return;
    // only copy register when src and dst are different
    GenerateMovStmt(dst, src);
}

void FunctionState::LoadFromImm(Tree *dst, Tree *src) {
    GenerateMovStmt(dst, src);
}

void FunctionState::GenerateLabelStmt(const char *l) {
    AddInst(new X86Inst(l, true));
}

void FunctionState::GenerateLabelStmt(Tree *t) {
    AddInst(new X86Inst(t->GetValue().AsLabel(), true));
}

void FunctionState::GenerateMovStmt(Tree *dst, Tree *src) {
    assert(!(dst->GetOpCode() == MEM && src->GetOpCode() == MEM) && "src and dst cannot both come from memory");
    // Keep this one-line function, since we might want 
    // to migrate to different operand sizes, so we will
    // be using movb, movw, movl, movq according to the
    // operands
    if (src->GetOpCode() == IMM)
        GenerateBinaryStmt("movabs", dst, src);
    else
        GenerateBinaryStmt("mov", dst, src);
}

void FunctionState::GenerateMovStmt(X86Operand *dst, X86Operand *src) {
    if (src->GetType() == OP_TYPE::X86Imm)
        GenerateBinaryStmt("movabs", dst, src);
    else
        GenerateBinaryStmt("mov", dst, src);
}

void FunctionState::GenerateBinaryStmt(const char *op_raw, Tree *dst, Tree *src, bool suffix) {
    // Keep this one-line function, since we might want 
    // to migrate to different operand sizes, so we will
    // be using suffixes b, w, l, q according to the
    // operands
    std::string op = std::string(op_raw);
    if (suffix) op = std::string(op_raw) + "q";

    AddInst(new X86Inst(op.c_str(), 
        dst->AsX86Operand(this), 
        src->AsX86Operand(this)
    ));
}

void FunctionState::GenerateBinaryStmt(const char *op_raw, X86Operand *dst, X86Operand *src, bool suffix) {
    // Keep this one-line function, since we might want 
    // to migrate to different operand sizes, so we will
    // be using suffixes b, w, l, q according to the
    // operands
    std::string op = std::string(op_raw);
    if (suffix) op = std::string(op_raw) + "q";
    AddInst(new X86Inst(op.c_str(), dst, src));
}

void FunctionState::GeneratePushStmt(Tree *t) {
    if (t->GetOpCode() == REG) {
        auto operand = new X86Operand(this, OP_TYPE::X86Reg, t->GetValue().AsVirtualReg());
        AddInst(new X86Inst("pushq", operand));
        freeList.push_back(operand);
    }
    else if (t->GetOpCode() == IMM) {
        auto operand = new X86Operand(this, OP_TYPE::X86Imm, t->GetValue().AsVirtualReg());
        AddInst(new X86Inst("pushq", operand));
        freeList.push_back(operand);
    }
}

void FunctionState::GeneratePushStmt(Register r) {
    auto operand = new X86Operand(this, r);
    AddInst(new X86Inst("pushq", operand));
    freeList.push_back(operand);
}

void FunctionState::GeneratePopStmt(Tree *t) {
    if (t->GetOpCode() == REG) {
        auto operand = new X86Operand(this, OP_TYPE::X86Reg, t->GetValue().AsVirtualReg());
        AddInst(new X86Inst("popq", operand));
        freeList.push_back(operand);
    }
    else if (t->GetOpCode() == IMM) {
        auto operand = new X86Operand(this, OP_TYPE::X86Imm, t->GetValue().AsVirtualReg());
        AddInst(new X86Inst("popq", operand));
        freeList.push_back(operand);
    }
}

void FunctionState::GeneratePopStmt(Register r) {
    auto operand = new X86Operand(this, r);
    AddInst(new X86Inst("popq", operand));
    freeList.push_back(operand);
}

void FunctionState::RecordLiveness(Tree *t) {
    t->RemoveRef();                      // decrease the reference counter

    if (!t->IsVirtualReg()) return;      // we only care about registers' references

#if 0
    std::cerr << "Virtual REG stop : " << t->GetValue().AsVirtualReg() << "\tRefCnt: " << t->GetRefCount() 
              << "\tLineNo: " << (assembly.size() - 1)<< std::endl;
#endif

    int reg = t->val.AsVirtualReg();
    if (t->GetRefCount() == 0) {
        // this register is dead now, we should set the endLine
        int endLine = assembly.size();    // ending at current size
        allocator.RecordLiveStop(reg, endLine);
    }
    // else this register is still live now, do not do anything
}

void FunctionState::RecordLiveStart(Tree *t) {
#if 0
    if (t->IsVirtualReg())
        std::cerr << "Virtual REG start: " << t->GetValue().AsVirtualReg() << "\tRefCnt: " << t->GetRefCount() 
                  << "\tLineNo: " << (assembly.size() - 1)<< std::endl;
#endif
    if (/*t->IsPhysicalReg() || */(t->IsVirtualReg() && t->GetRefCount() > 0)) {
        int reg = t->val.AsVirtualReg();
        int startLine = assembly.size();
        allocator.RecordLiveStart(reg, startLine);
    }
    // this register is dead if refcnt == 0
}

// ========================================================
// register allocation
int  FunctionState::CreateSpill(std::ostream &out, int reg_idx) {
    X86Operand operand(this, reg_idx);
    X86Inst inst("pushq", &operand);
    out << inst << "\t; spill " << registers[reg_idx] << std::endl;
    local_bytes += 8;
    return -local_bytes;
}

void FunctionState::RestoreSpill(std::ostream &out, int reg_idx, int offset) {
    X86Operand operand1(this, reg_idx);
    X86Operand *operand2 = GetLocalMemoryAddress(offset);
    X86Inst inst("movq", &operand1, operand2);
    out << inst << "\t; restore " << registers[reg_idx] << std::endl;
    delete operand2;
}
