#define VERBOSE  0

int OP_LABEL(NODEPTR p) {
    if (p) {
        // there is a hidden bug somewhere that returns strange value for p->op
        // not sure where it is, but expression tree building seems fine to me
        // therefore this is a hack to change anything bad to DUMMY, since the
        // only possible bad thing in this system is DUMMY
        if (p->op > LABEL) return DUMMY;
        return p->op;
    }
    std::cerr << "NODEPTR is null for OP_LABEL" << std::endl;
    exit(EXIT_FAILURE);
}

using namespace llvm;

static cl::opt<std::string>
InputFilename(cl::Positional, cl::desc("<input bitcode>"), cl::init("-"));

static cl::opt<std::string>
OutputFilename("o", cl::desc("<output filename>"), cl::value_desc("filename"));

static cl::opt<int>
NumRegs("num_regs", cl::desc("<number of registers available>"), cl::init(16));

/* burm_trace - print trace message for matching p */
static void burm_trace(NODEPTR p, int eruleno, COST cost) {
    if (shouldTrace)
        std::cerr << p << " matched " << burm_string[eruleno] << " = " << eruleno << " with cost " << cost.cost << std::endl;
}

Tree tree(int op, Tree l, Tree r, VALUE v) {
	Tree t = (Tree) malloc(sizeof *t);
	t->op = op;
	t->kids[0] = l; t->kids[1] = r; t->kids[2] = nullptr;
	t->val = v;
	t->x.state = 0;
    t->level = 0;
    t->refcnt = 0;
	return t;
}

void gen(NODEPTR p, FunctionState *fstate) {
    p->computed = true;
	if (burm_label(p) == 0)
		std::cerr << "=== NO COVER ===\n";
	else {
		stmt_action(p->x.state, fstate);
		if (shouldCover != 0)
			dumpCover(p, 1, 0);
	}
}

TreeWrapper::TreeWrapper(int opcode, Tree l, Tree r): nchild(0) {
    t = tree(opcode, l, r);
    t->refcnt = 0;
    t->level = 1;
    if (l) { t->level = std::max(t->level, 1 + l->level); nchild++; }
    if (r) { t->level = std::max(t->level, 1 + r->level); nchild++; }
}
TreeWrapper::~TreeWrapper() {
    free(t);
    // Stupid design in TreeReference causes me to use a freeList for cleaning
    for (Tree ct: freeList)
        free(ct);
}
void TreeWrapper::SetChild(int n, Tree ct) {
    if (n < 3) {
        if (!t->kids[n] && ct) nchild++;     // was nullptr, but not null now, increment the nchild counter
        if (ct) t->level = std::max(t->level, 1 + ct->level);
        t->kids[n] = ct;
    } else {
        // TODO: IMPLEMENT A WAY TO STORE MORE THAN 2 CHILDREN
        errs() << "SetChild for n=" << n << " is not implemented yet!\n";
    }
}
Tree TreeWrapper::GetChild(int n) {
    if (n < 3) {
        return t->kids[n];
    } else {
        // TODO: IMPLEMENT A WAY TO FETCH MORE THAN 2 CHILDREN
        errs() << "GetChild for n=" << n << " is not implemented yet!\n";
        return nullptr;
    }
}

void TreeWrapper::CastInt(int n, ConstantInt *cnst) {
    freeList.push_back(tree(IMM, nullptr, nullptr));
    Tree ct = freeList.back();
    const APInt integer = cnst->getValue();
    unsigned bitWidth = cnst->getBitWidth();
    if (integer.isSignedIntN(bitWidth)) {
        switch (bitWidth) {
            case 1:
                ct->val.val.b = (bool) cnst->getSExtValue();
                break;
            case 8:
                ct->val.val.i8s = (int8_t)cnst->getSExtValue();
                break;
            case 16:
                ct->val.val.i16s = (int16_t)cnst->getSExtValue();
                break;
            case 32:
                ct->val.val.i32s = (int32_t)cnst->getSExtValue();
                break;
            case 64:
                ct->val.val.i64s = (int64_t)cnst->getSExtValue();
                break;
            default:
                errs() << "CAST CONSTANT INT FAILURE\n";
                exit(EXIT_FAILURE);
        }
    }
    else {
        switch (bitWidth) {
            case 1:
                ct->val.val.b = (bool) cnst->getSExtValue();
                break;
            case 8:
                ct->val.val.i8u = (uint8_t)cnst->getZExtValue();
                break;
            case 16:
                ct->val.val.i16u = (uint16_t)cnst->getZExtValue();
                break;
            case 32:
                ct->val.val.i32u = (uint32_t)cnst->getZExtValue();
                break;
            case 64:
                ct->val.val.i64u = (uint64_t)cnst->getZExtValue();
                break;
            default:
                errs() << "CAST CONSTANT INT FAILURE\n";
                exit(EXIT_FAILURE);
        }
    }
    this->SetChild(n, ct);
}

void TreeWrapper::CastFP(int n, ConstantFP *cnst) {
    freeList.push_back(tree(IMM, nullptr, nullptr));
    Tree ct = freeList.back();
    // for now, assume all floating point uses 64 bit double
    ct->val.val.f64 = (double) cnst->getValueAPF().convertToDouble();
    this->SetChild(n, ct);
}

void TreeWrapper::DisplayTree(Tree t, int indent) const {
    if (t == nullptr) return;
    for (int i = 0; i < 2 * indent; i++)
        errs() << " ";
    switch (t->op) {
    case DUMMY:
        errs() << "op: " << "dummy" << "\tval: " << t->val.val.i32s << "\n";
        break;
    case LABEL:
        errs() << "op: " << "label" << "\tval: " << t->val.val.i32s << "\n";
        break;
    case REG:
        errs() << "op: " << "reg" << "\tval: " << t->val.val.i32s << "\n";
        break;
    case IMM:
        errs() << "op: " << "imm" << "\tval: " << t->val.val.i32s << "\n";
        break;
    case MEM:
        errs() << "op: " << "mem" << "\tval: " << t->val.val.i32s << "\n";
        break;
    default:
        errs() << "op: " << Instruction::getOpcodeName(t->op) << "(" << t->op << ")" << "\tval: " << t->val.val.i32s << "\n";
    }
    for (int i = 0; i < 3; i++) {
        if (t->kids[i]) {
            DisplayTree(t->kids[i], indent + 1);
        }
    }
}

void BasicBlockToExprTrees(FunctionState &fstate,
        std::vector<TreeWrapper*> &treeList, BasicBlock &bb) {

    TreeWrapper dummy(DUMMY, nullptr, nullptr);
    
    for (auto inst = bb.begin(); inst != bb.end(); inst++) {
        TreeWrapper *t = new TreeWrapper(inst->getOpcode());
        Instruction &instruction = *inst;
        t->SetInst(&instruction);

#if VERBOSE
        errs() << "\n";
        instruction.print(errs());
        errs() << "\n";
#endif
#if VERBOSE > 1
        errs() << "OperandNum: " << instruction.getNumOperands() << "\t";
        errs() << "opcode: " << instruction.getOpcode() << "\n";
#endif

        // HACK for making BranchInst all 3 address code
        // ---------------------------------------------
        if (instruction.getOpcode() == Br) {
            // first fill the branch inst with dummy integer
            t->SetChild(1, dummy.GetTree());
            t->SetChild(2, dummy.GetTree());
        }
        // ---------------------------------------------

        int num_operands = instruction.getNumOperands();
        for (int i = 0; i < num_operands; i++) {
            Value *v = instruction.getOperand(i);
#if VERBOSE > 1
            errs() << "\tOperand " << i << ":\t";
            v->print(errs());
            errs() << "\n";
#endif

            if (Constant *def = dyn_cast<Constant>(v)) {
                // check if the operand is a constant
                if (ConstantInt *cnst = dyn_cast<ConstantInt>(v)) {
                    // check if the operand is a constant int
                    t->CastInt(i, cnst);
                }
                else if (ConstantFP *cnst = dyn_cast<ConstantFP>(v)) {
                    // check if the operand is a constant int
                    t->CastFP(i, cnst);
                }
                else if (ConstantExpr *cnst = dyn_cast<ConstantExpr>(v)) {
                    // check if the operand is a constant int
                    // errs() << "FOUND CONST EXPR:\t" << *cnst << "\n";
                    errs() << "NOT IMPLEMENTED CONST EXPR\n";
                    exit(EXIT_FAILURE);
                }
                // ... There are many kinds of constant, right now we do not deal with them ...
                else {
                    // this is bad and probably needs to terminate the execution
                    errs() << "NOT IMPLEMENTED OTHER CONST TYPES:\t"; instruction.print(errs()); errs() << "\n";
                    exit(EXIT_FAILURE);
                }
            }
            else if (Instruction *def = dyn_cast<Instruction>(v)) {
                // check if we cant find operand's definition
                TreeWrapper *wrapper = fstate.FindFromTreeMap(def);
                assert(wrapper && "operands must be previously defined");
                t->SetChild(i, wrapper->GetTreeWrapper());      // automatically increase the refcnt
            }
            else if (BasicBlock *block = dyn_cast<BasicBlock>(v)) {
                if (instruction.getOpcode() == Br) {
                    TreeWrapper *wrapper = fstate.FindLabel(block);
                    assert(wrapper && "FindLabel should never fail");
                    t->SetChild(i, wrapper->GetTreeWrapper()); // automatically increase the refcnt
                }
                else {
                    errs() << "Unhandle-able basic block operand! Quit\n";
                    exit(EXIT_FAILURE);
                }
            }
            else {
                // TODO: write code to handle these situations
                errs() << "Unhandle-able instruction operand! Quit\n";
                exit(EXIT_FAILURE);
            }
            // errs() << "num operands:\t" << num_operands << "\n"; 
            // t->DisplayTree();
        } //end of operand loop

        // store the current tree in map
        fstate.AddToTreeMap(&instruction, t);
        treeList.push_back(t);
    } // end of instruction loop in a basic block
}

/**
 * Generate assembly for a single function
 * */
void FunctionToAssembly(Function &func) {

    // prepare a function state container
    // to store function information, such as
    // local variables, free registers, etc
    FunctionState fstate(func.getName(), NumRegs);

    Function::BasicBlockListType &basic_blocks = func.getBasicBlockList();
    // === First pass: Collect basic block info, which basic block will need label
    for (BasicBlock &bb : basic_blocks) {
        for (auto inst = bb.begin(); inst != bb.end(); inst++) {
            int num_operands = inst->getNumOperands();
            for (int i = 0; i < num_operands; i++) {
                Value *v = inst->getOperand(i);
                if (!dyn_cast<Instruction>(v))
                    if (BasicBlock *block = dyn_cast<BasicBlock>(v))
                        fstate.CreateLabel(block);
            } // end of operand loop
        } // end of inst loop
    } // end of BB loop
    // ------------------------------------------------------------------------

    // === Second Pass: collect instruction information, build expr tree, generate assembly
    std::vector<TreeWrapper*> treeList;
    for (BasicBlock &bb : basic_blocks) {
        int size = treeList.size();
        if (TreeWrapper * wrapper = fstate.FindLabel(&bb)) {
            fstate.GenerateLabelStmt(wrapper->GetValue());
        }
        BasicBlockToExprTrees(fstate, treeList, bb);

        // iterate through tree list for each individual instruction tree
        // replace the complicated/common tree expression with registers
        for (int i = size; i < treeList.size(); i++) {
            TreeWrapper *t = treeList[i];
#if VERBOSE > 2
            errs() << Instruction::getOpcodeName(t->GetOpCode()) << "\tLEVEL:\t" << t->GetLevel() << "\tRefCount:\t" << t->GetRefCount() << "\n";
#endif
            // check if this tree satisfies the condition for saving into register
            if (t->GetRefCount() == 0/* || t->GetRefCount() > 2*/) {
#if VERBOSE > 2
                t->DisplayTree();
#endif
                gen(t->GetTree(), &fstate);
            }
        } // end of TreeWrapper iteration
    } // end of basic block loop
    // ------------------------------------------------------------------------

    // === Third Pass: analyze virtual register live range, allocate machine register and output assembly file
    fstate.PrintAssembly(std::cerr);

    // clean up
    for (TreeWrapper *t : treeList) delete t;
}

int main(int argc, char *argv[])
{
#if 0
    // Tree t = tree(Add, 
    //             tree(IMM, nullptr, nullptr, 1), 
    //             tree(IMM, nullptr, nullptr, 2), 
    //         1);
    // Tree t = tree(Store, 
    //             tree(IMM, nullptr, nullptr, 2), 
    //             tree(REG, nullptr, nullptr, 1), 
    //         1);
    Tree t = tree(Store,
                tree(Load,
                    tree(IMM, nullptr, nullptr, 10),
                    tree(Alloca, nullptr, nullptr, 1)),
                tree(REG, nullptr, nullptr, 2),
            1);
    gen(t);
#else
    // parse arguments from command line
    cl::ParseCommandLineOptions(argc, argv, "llc-olive\n");

    // prepare llvm context to read bitcode file
    LLVMContext context;
    SMDiagnostic error;
    std::unique_ptr<Module> module = parseIRFile(StringRef(InputFilename.c_str()), error, context);

    // obtain a function list in module, and iterate over function
    Module::FunctionListType &function_list = module->getFunctionList();
    for (Function &func : function_list) {
        FunctionToAssembly(func);
    }
#endif

    return 0;
}
