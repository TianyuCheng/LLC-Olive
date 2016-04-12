#include "FunctionState.h"
#define DEBUG 0

FunctionState::FunctionState(std::string name, int n, int f, int l)
    : function_name(name), function_id(f), label_id(l), local_bytes(8), num_args(0), allocator(n), current_line(0), num_regs(n)
{
    // initialize function state here
    // local_bytes is initiated to 8 for saved rip
    local_bytes = 4 * 8;    // space for 4 callee_saved registers, 3 out of 7 are special purpose, like %rbp, %rsp, we will manually restore them
}

FunctionState::~FunctionState() {
    // de-initialize function state here
    for (X86Inst *inst : assembly)
        delete inst;
    for (auto it = labelMap.begin(); it != labelMap.end(); ++it)
        delete it->second;
    // for (auto operand : freeList)
    //     delete operand;
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
    out << "\t.globl " << function_name << std::endl;
    out << "\t.type " << function_name  << ", @function"<< std::endl;

    // prolog
    out << function_name  << ":" << std::endl;
    out << "\tpushq\t%rbp" << std::endl;
    out << "\tmovq\t%rsp,\t%rbp" << std::endl;
    // callee-save registers
    for (int i = 3; i < 7; i++)
        out << "\tpushq\t%" << registers[callee_saved_regs[i]] << "\t\t# push callee-saved reg" << std::endl;

    if (local_bytes - 4 * 8 > 0)
        out << "\tsubq\t$" << (local_bytes - 4 * 8) << ",\t%rsp" << std::endl;

    std::vector<Register> regs;

    // TODO: remember to print function begin and ends
    for (X86Inst *inst : assembly) {
        allocator.ResetNoSpills();          // reset registers that cannot be spilt
        inst->ResolveRegs(this, out);

        if (inst->IsCallBegin()) {
            allocator.DisableSpill(this, out);
            regs.clear();
            // find which registers to preserve and restore
            // we skip RAX, because this register will be used as special purpose register
            for (int i = 1; i < 9; i++) {
                Register r = caller_saved_regs[i];
                if (RegisterInUse(r))
                    regs.push_back(r);
            }
            // push registers
            for (auto it = regs.begin(); it != regs.end(); it++)
                out << "\tpushq\t%" << registers[*it] << "\t\t# push caller-save reg" << std::endl;
        }
        else if (inst->IsCallEnd()) {
            allocator.EnableSpill(this, out);
            // push registers
            for (auto it = regs.rbegin(); it != regs.rend(); it++)
                out << "\tpopq\t%" << registers[*it] << "\t\t# pop caller-save reg" << std::endl;
        }
        else if (inst->IsCall()) {
            // print call instruction
            out << "\txorq\t%rax,\t%rax" << std::endl;      // some functions, like printf requires rax to be 0
#if DEBUG
            out << *inst << "\t # Line " << current_line << std::endl;
#else
            out << *inst << std::endl;
#endif
        }
        else {
#if DEBUG
            out << *inst << "\t # Line " << current_line << std::endl;
#else
            out << *inst << std::endl;
#endif
        }
        current_line++;
    }

    // epilog
    out << ".LFE" << function_id << ":" << std::endl;
    if (local_bytes - 4 * 8 > 0)
        out << "\taddq\t$" << (local_bytes - 4 * 8) << ",\t%rsp" << std::endl;
    // restore registers
    for (int i = 6; i >= 3; i--)
        out << "\tpopq\t%" << registers[callee_saved_regs[i]] << "\t\t# pop callee-saved reg" << std::endl;
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
    treeLabel->SetValue(label_id++);
    labelMap.insert(std::pair<llvm::BasicBlock*, Tree*>(bb, treeLabel));
    return treeLabel;
}

void FunctionState::CreateLocal(Tree *t, int bytes) {
    // already allocated
    auto it = locals.find(t);
    if (it != locals.end()) return;

    // now allocate local variable
    if (bytes % 8 != 0) bytes += 8 - bytes % 8;
    local_bytes += bytes;
    X86Operand *local = new X86Operand(this, OP_TYPE::X86Mem, 
            new X86Operand(this, RBP),                     // base_address, should be rbp
            new X86Operand(this, OP_TYPE::X86Imm, 0),      // displacement
            0,                                             // multiplier    
            -local_bytes);                       // negate because locals are below rbp
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
            8 * (num_args - 6 + 2));                       // offset
        locals.insert(std::pair<Tree*, X86Operand*>(t, op));
    }
    argsMap.insert(std::pair<llvm::Argument*, Tree*>(arg, t));
    num_args++;
}

void FunctionState::CreateVirtualReg(Tree *t) {
    // do nothing for PhiNode, because the register value has been pre-allocated
    if (t->IsPhiNode()) { return; }
    // if already allocated, do nothing
    if (t->HasValue()) { return; }

    using namespace llvm;
    int v = allocator.CreateVirtualReg();
    // llvm::Value *val = t->GetLLVMValue();
    // assert(val && "llvm value should not be null");
    t->SetValue(v);
    t->UseAsVirtualRegister();
    RecordLiveStart(t);                 // newly allocated virtual register must be added to liveness
}

void FunctionState::CreatePhysicalReg(Tree *t, Register r) {
    t->SetValue(r);
    t->UseAsPhysicalRegister();
    RecordLiveStart(t);                 // newly allocated virtual register must be added to liveness
}

void FunctionState::AssignVirtualReg(Tree *lhs, Tree *rhs) {
    // if (rhs->IsVirtualReg()) std::cerr << "FOUND RHS TO BE A PHYSICAL REG" << std::endl;
    // if (rhs->IsPhysicalReg()) std::cerr << "FOUND RHS TO BE A PHYSICAL REG" << std::endl;

    // do nothing for PhiNode, because the register value has been pre-allocated
    if (lhs->IsPhiNode()) { return; }
    // if already allocated, do nothing
    if (lhs->HasValue()) { return; }

    lhs->UseAsVirtualRegister();

    if (rhs->IsVirtualReg() && rhs->GetRefCount() == 1) {
        // this register is about to be free, we can reuse it
        lhs->val = rhs->val;
        lhs->UseAsVirtualRegister();
    }
    else {
        // we will still be using rhs in the future, 
        // so better not overwrite the rhs
        CreateVirtualReg(lhs);      // assign a virtual register to the inst
        GenerateMovStmt(lhs, rhs);
    }
}

void FunctionState::LoadEffectiveAddress(Tree *dst, Tree *src) {
    if (dst->IsMemory() && src->IsMemory()) {
        Tree *t1 = (new Tree(REG))->GetTreeRef();
        CreateVirtualReg(t1);
        GenerateBinaryStmt("lea", t1, src, "q");
        GenerateBinaryStmt("mov", dst, t1, "q");
    }
    else {
        GenerateBinaryStmt("lea", dst, src, "q");
    }
}

void FunctionState::GenerateLabelStmt(const char *l) {
    AddInst(new X86Inst(l, true));
}

void FunctionState::GenerateLabelStmt(Tree *t) {
    AddInst(new X86Inst(t->GetValue().AsLabel(), true));
}

void FunctionState::GenerateMovStmt(Tree *dst, Tree *src, const char *suffix) {
    // special cases:
    // 1. dst and src are both pointers
    if (dst->IsMemory() && src->IsMemory()) {
        Tree *t1 = (new Tree(REG))->GetTreeRef();
        CreateVirtualReg(t1);
        GenerateBinaryStmt("mov", t1, src, suffix);
        GenerateBinaryStmt("mov", dst, t1, suffix);
    }
    else {
        if (dst->IsVirtualReg() && src->IsVirtualReg() && dst->GetValue().AsVirtualReg() == src->GetValue().AsVirtualReg())
            return;
        if (dst->IsPhysicalReg() && src->IsPhysicalReg() && dst->GetValue().AsVirtualReg() == src->GetValue().AsVirtualReg())
            return;
        GenerateBinaryStmt("mov", dst, src, suffix);
    }
}

void FunctionState::GenerateMovStmt(X86Operand *dst, X86Operand *src, const char *suffix) {
    GenerateBinaryStmt("mov", dst, src, suffix);
}

void FunctionState::GenerateStmt(const char *op) {
    AddInst(new X86Inst(op, nullptr, nullptr));
}

void FunctionState::GenerateUnaryStmt(const char *op_raw, Tree *src, const char *suffix) {
    std::string op = std::string(op_raw);
    if (suffix) op = std::string(op_raw) + suffix;

    AddInst(new X86Inst(op.c_str(), 
        src->AsX86Operand(this), nullptr
    ));
}

void FunctionState::GenerateBinaryStmt(const char *op_raw, Tree *dst, Tree *src, const char *suffix) {
    // Keep this one-line function, since we might want 
    // to migrate to different operand sizes, so we will
    // be using suffixes b, w, l, q according to the
    // operands
    std::string op = std::string(op_raw);
    if (suffix) op = std::string(op_raw) + suffix;

    AddInst(new X86Inst(op.c_str(), 
        dst->AsX86Operand(this), 
        src->AsX86Operand(this)
    ));
}

void FunctionState::GenerateBinaryStmt(const char *op_raw, X86Operand *dst, X86Operand *src, const char *suffix) {
    // Keep this one-line function, since we might want 
    // to migrate to different operand sizes, so we will
    // be using suffixes b, w, l, q according to the
    // operands
    std::string op = std::string(op_raw);
    if (suffix) op = std::string(op_raw) + suffix;
    AddInst(new X86Inst(op.c_str(), dst, src));
}

void FunctionState::GeneratePushStmt(Tree *t) {
    switch (t->GetOpCode()) {
        case Add:
        case Sub:
        case Mul:
        case UDiv:
        case SDiv:
        case Load:
        case REG:
            {
                auto operand = new X86Operand(this, OP_TYPE::X86Reg, t->GetValue().AsVirtualReg());
                AddInst(new X86Inst("pushq", operand));
                freeList.push_back(operand);
                break;
            }
        case IMM:
            {
                auto operand = new X86Operand(this, OP_TYPE::X86Imm, t->GetValue().AsVirtualReg());
                AddInst(new X86Inst("pushq", operand));
                freeList.push_back(operand);
                break;
            }
        case MEM:
            {
                auto operand = GetLocalMemoryAddress(t);
                AddInst(new X86Inst("pushq", operand));
                freeList.push_back(operand);
                break;
            }
        default:
            std::cerr << "GeneratePushStmt: " << t->GetOpCode() << std::endl;
            assert(false && "inalid push operand type");
    }
    local_bytes += 8;
}

void FunctionState::GeneratePushStmt(Register r) {
    auto operand = new X86Operand(this, r);
    AddInst(new X86Inst("pushq", operand));
    freeList.push_back(operand);
    local_bytes += 8;
}

void FunctionState::GeneratePopStmt(Tree *t) {
    switch (t->GetOpCode()) {
        case Add:
        case Sub:
        case Mul:
        case UDiv:
        case SDiv:
        case Load:
        case REG:
            {
                auto operand = new X86Operand(this, OP_TYPE::X86Reg, t->GetValue().AsVirtualReg());
                AddInst(new X86Inst("popq", operand));
                freeList.push_back(operand);
                break;
            }
        case IMM:
            {
                auto operand = new X86Operand(this, OP_TYPE::X86Imm, t->GetValue().AsVirtualReg());
                AddInst(new X86Inst("popq", operand));
                freeList.push_back(operand);
                break;
            }
        case MEM:
            {
                auto operand = GetLocalMemoryAddress(t);
                AddInst(new X86Inst("popq", operand));
                freeList.push_back(operand);
                break;
            }
        default:
            std::cerr << "GeneratePopStmt: " << t->GetOpCode() << std::endl;
            assert(false && "inalid push operand type");
    }
    local_bytes -= 8;
}

void FunctionState::GeneratePopStmt(Register r) {
    auto operand = new X86Operand(this, r);
    AddInst(new X86Inst("popq", operand));
    freeList.push_back(operand);
    local_bytes -= 8;
}

void FunctionState::RecordLiveness(Tree *t) {
    t->RemoveRef();                      // decrease the reference counter

    if (t->IsPhiNode()) {
        // this must be a phi node,
        // find the real reference for phi
        t = t->GetParent();
        assert(t && "phinode must have its parent");
    }

    if (!t->IsVirtualReg()) return;      // we only care about registers' references
    if (t->IsPhysicalReg()) return;      // we do not record the physical registers' liveness

#if DEBUG
    std::cerr << "Virtual REG stop\t OPCODE: " << t->GetOpCode() << "\tVirtual Reg: "  << t->GetValue().AsVirtualReg() << "\tRefCnt: " << t->GetRefCount() 
              << "\tLineNo: " << (assembly.size() - 1)<< std::endl;
    assert(t->GetRefCount() >= 0);
#endif

    int reg = t->val.AsVirtualReg();
    // record every point where this register is referenced
    int endLine = assembly.size();    // ending at current size
    allocator.RecordLiveStop(reg, endLine);
    // else this register is still live now, do not do anything
}

void FunctionState::RecordLiveStart(Tree *t) {
    if (t->IsPhiNode()) {
        // this must be a phi node,
        // find the real reference for phi
        t = t->GetParent();
        assert(t && "phinode must have its parent");
    }
    if (!t->IsPhysicalReg() && (t->IsVirtualReg()/* && t->GetRefCount() > 0*/)) {
#if DEBUG
    if (t->IsVirtualReg())
        std::cerr << "Virtual REG start: " << t->GetValue().AsVirtualReg() << "\tRefCnt: " << t->GetRefCount() 
                  << "\tLineNo: " << (assembly.size() - 1)<< std::endl;
#endif
        int reg = t->val.AsVirtualReg();
        int startLine = assembly.size();
        allocator.RecordLiveStart(reg, startLine);
    }
    // this register is dead if refcnt == 0
}

// ========================================================
// register allocation
int  FunctionState::CreateSpill(std::ostream &out, int reg_idx) {
    // // std::cerr << "Spill out physical reg: " << reg_idx << std::endl;

    local_bytes += 8;       // pre-assign space for register spill
    int offset = -local_bytes;

    X86Operand *operand1 = GetLocalMemoryAddress(offset);
    X86Operand operand2(this, reg_idx);
    X86Inst inst("movq", operand1, &operand2);
    out << inst << "\t# spill out reg " << registers[reg_idx] << " at OFFSET: " << offset << std::endl;
    delete operand1;
    return -local_bytes;
}

void FunctionState::RestoreSpill(std::ostream &out, int reg_idx, int offset) {
    // std::cerr << "restore physical reg: " << reg_idx << " from offset: " << offset << std::endl;
    X86Operand operand1(this, reg_idx);
    X86Operand *operand2 = GetLocalMemoryAddress(offset);
    X86Inst inst("movq", &operand1, operand2);
    out << inst << "\t# restore spilled reg " << registers[reg_idx] << " at OFFSET: " << offset << std::endl;
    delete operand2;
}
