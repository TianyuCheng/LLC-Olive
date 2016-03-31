#define VERBOSE  0

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

void gen(NODEPTR p, FunctionState *fstate) {
    p->SetComputed(true);
	if (burm_label(p) == 0)
		std::cerr << "=== NO COVER ===\n";
	else {
		stmt_action(p->x.state, fstate);
		if (shouldCover != 0)
			dumpCover(p, 1, 0);
	}
}

void BasicBlockToExprTrees(FunctionState &fstate,
        std::vector<Tree*> &treeList, BasicBlock &bb) {

    for (auto inst = bb.begin(); inst != bb.end(); inst++) {
        Tree *t = new Tree(inst->getOpcode());
        Instruction &instruction = *inst;
        t->SetInst(&instruction);

#if VERBOSE
        errs() << "\n";
        instruction.print(errs());
        errs() << "\n";
#endif
#if VERBOSE > 1
        errs() << "---------------------------------------------------------------\n";
        errs() << "OperandNum: " << instruction.getNumOperands() << "\t";
        errs() << "opcode: " << instruction.getOpcode() << "\n";
#endif

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
                    t->CastInt(cnst);
                }
                else if (ConstantFP *cnst = dyn_cast<ConstantFP>(v)) {
                    // check if the operand is a constant int
                    t->CastFP(cnst);
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
                Tree *wrapper = fstate.FindFromTreeMap(def);
                assert(wrapper && "operands must be previously defined");
                t->AddChild(wrapper->GetTreeRef());      // automatically increase the refcnt
            }
            else if (BasicBlock *block = dyn_cast<BasicBlock>(v)) {
                if (instruction.getOpcode() == Br) {
                    Tree *wrapper = fstate.FindLabel(block);
                    assert(wrapper && "FindLabel should never fail");
                    t->AddChild(wrapper->GetTreeRef()); // automatically increase the refcnt
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
        } //end of operand loop

        // HACK for making BranchInst all 3 address code
        // ---------------------------------------------
        if (instruction.getOpcode() == Br) {
            if (t->GetNumKids() == 1) {
                // first fill the branch inst with dummy node
                t->AddChild(new Tree(DUMMY));
                t->AddChild(new Tree(DUMMY));
            }
        }
        // ---------------------------------------------

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
    std::vector<Tree*> treeList;
    for (BasicBlock &bb : basic_blocks) {
        int size = treeList.size();
        if (Tree * wrapper = fstate.FindLabel(&bb)) {
            fstate.GenerateLabelStmt(wrapper->GetValue());
        }
        BasicBlockToExprTrees(fstate, treeList, bb);

        // iterate through tree list for each individual instruction tree
        // replace the complicated/common tree expression with registers
        for (int i = size; i < treeList.size(); i++) {
            Tree *t = treeList[i];
#if VERBOSE > 2
            errs() << Instruction::getOpcodeName(t->GetOpCode()) << "\tLEVEL:\t" << t->GetLevel() << "\tRefCount:\t" << t->GetRefCount() << "\n";
#endif
            // check if this tree satisfies the condition for saving into register
            if (t->GetRefCount() == 0/* || t->GetRefCount() > 2*/) {
#if VERBOSE > 2
                t->DisplayTree();
#endif
                gen(t, &fstate);
            }
        } // end of Tree iteration
    } // end of basic block loop
    // ------------------------------------------------------------------------

    // === Third Pass: analyze virtual register live range, allocate machine register and output assembly file
    fstate.PrintAssembly(std::cerr);

    // clean up
    for (Tree *t : treeList) delete t;
}

int main(int argc, char *argv[])
{
    // parse arguments from command line
    cl::ParseCommandLineOptions(argc, argv, "llc-olive\n");

    // prepare llvm context to read bitcode file
    LLVMContext context;
    SMDiagnostic error;
    std::unique_ptr<Module> module = parseIRFile(StringRef(InputFilename.c_str()), error, context);

    errs() << "Num-Regs: " << NumRegs << "\n";

    // obtain a function list in module, and iterate over function
    Module::FunctionListType &function_list = module->getFunctionList();
    for (Function &func : function_list) {
        FunctionToAssembly(func);
    }

    return 0;
}
