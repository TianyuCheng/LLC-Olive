#include "FunctionState.h"

FunctionState::FunctionState(std::string name, int n, int l)
    : function_name(name), label(l), local_bytes(8), num_args(0), allocator(n)
{
    // initialize function state here
    // local_bytes is initiated to 8 for saved rip

    // // preserve registers
    // for (int i = 0; i < 7; i++)
    //     GeneratePushStmt(preserve_regs[i]);
}

FunctionState::~FunctionState() {
    // de-initialize function state here
    for (X86Inst *inst : assembly)
        delete inst;
    for (auto it = labelMap.begin(); it != labelMap.end(); ++it)
        delete it->second;
    for (auto it = liveness.begin(); it != liveness.end(); ++it)
        delete it->second;
    for (auto operand : freeList) {
        delete operand;
    }
}

void FunctionState::PrintAssembly(std::ostream &out, RegisterAllocator &ra) {
    // out << "#####################################################" << std::endl;
    // this->PrintLiveness(out);
    // out << "#####################################################" << std::endl;
    // print assembly to file
    
    // pass liveness analysis to register allocator
    /*
       allocator.set_intervals(liveness);
    allocator.linearScanAllocate();
    */
    // virtual2value = ra.get_virtual2value();

    // print function entrance
    // TODO: make these a part of the assembly code
    if (std::string(function_name) == std::string("main"))
        out << "\t.globl" << std::endl;

    // prolog
    out << function_name  << ":" << std::endl;
    out << "\tpushq\t%rbp" << std::endl;
    out << "\tsubq\t%rsp, $" << local_bytes << std::endl;

    // // restore registers
    // for (int i = 6; i >= 0; i++)
    //     GeneratePopStmt(preserve_regs[i]);

    // epilog
    std::stringstream ss;
    ss << "." << function_name << "End";
    std::string s = ss.str();
    GenerateLabelStmt(s.c_str());
    RestoreStack();
    AddInst(new X86Inst("leave"));
    AddInst(new X86Inst("ret"));

    // TODO: remember to print function begin and ends
    for (X86Inst *inst : assembly) {
        X86Operand *dst = inst->GetDst();
        X86Operand *src = inst->GetSrc();
        if (dst && dst->IsVirtualReg()) {
            llvm::Value *v = virtual2value[dst->GetVirtualReg()];
            // call allocator, print spill if needed
            if (llvm::Instruction *inst = dyn_cast<Instruction>(v)) {
                ;
            }
        }
        if (src && src->IsVirtualReg()) {
            llvm::Value *v = virtual2value[src->GetVirtualReg()];
            // call allocator, print spill if needed
        }
        out << *inst;
    }

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

void FunctionState::RestoreStack() {
    if (local_bytes > 0)
        assembly.push_back(new X86Inst("addq",
                new X86Operand(this, RSP),   // should be rsp
                new X86Operand(this, OP_TYPE::X86Imm, local_bytes)
        ));
}

void FunctionState::CreateArgument(llvm::Argument *arg) {
    Tree *t = nullptr;
    X86Operand *op = nullptr;

    if (num_args < 6) {                 // first six parameters are passed in registers
        t = new Tree(REG, params_regs[num_args]);
        t->SetLLVMValue(arg);
        CreatePhysicalReg(t, params_regs[num_args]);
        // insert into liveness: should be live from start
        int startLine = assembly.size();
        liveness.insert(std::pair<int, LiveRange*>(t->GetValue().AsVirtualReg(), new LiveRange(startLine)));
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
    int v = virtual2value.size();
    llvm::Value *val = t->GetLLVMValue();
    if (!val) {
        std::cerr << "VIRTUAL REG NULL VALUE (OP): " << t->GetOpCode() << std::endl;
    }
    assert(val && "llvm value should not be null");
    virtual2value.push_back(val);
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
    GenerateBinaryStmt("mov", dst, src);
}

void FunctionState::GenerateMovStmt(X86Operand *dst, X86Operand *src) {
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
    t->RemoveRef();                // decrease the reference counter

#if 0
    if (!t->IsVirtualReg()) return;      // we only care about registers' references

    int reg = t->val.AsVirtualReg();
    if (t->GetRefCount() == 0) {
        // this register is dead now, we should set the endLine
        auto it = liveness.find(reg);
        if (it == liveness.end()) {
            t->DisplayTree();
        }
        if (it == liveness.end())
            this->PrintAssembly(std::cerr);
        assert(it != liveness.end() && "a dead register should have already in the liveness analysis");

        LiveRange *range = it->second;
        int endLine = assembly.size() - 1;    // ending at current size
        range->endpoint = endLine;
        liveness.insert(std::pair<int, LiveRange*>(reg, range));
    }
    // else this register is still live now, do not do anything
#endif
}

void FunctionState::PrintLiveness(std::ostream &out) {
#if 0
    for (auto it = liveness.begin(); it != liveness.end(); ++it) {
        LiveRange *range = it->second;
        out << "Virtual Register " << it->first << ":\t" 
            << "start:\t" << range->startpoint << "\t"
            << "end:\t"   << range->endpoint << std::endl;
    }
#endif
}

void FunctionState::RecordLiveStart(Tree *t) {
#if 0
    if (t->IsPhysicalReg() || (t->IsVirtualReg() && t->GetRefCount() > 0)) {
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
#endif
}
