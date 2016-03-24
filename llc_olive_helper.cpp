/* burm_trace - print trace message for matching p */
static void burm_trace(NODEPTR p, int eruleno, COST cost) {
    if (shouldTrace)
        std::cerr << "0x" << p << " matched " << burm_string[eruleno] << " = " << eruleno << " with cost " << cost.cost << "\n";
}


static Tree tree(int op, Tree l, Tree r, VALUE v = 0) {
	Tree t = (Tree) malloc(sizeof *t);
	t->op = op;
	t->kids[0] = l; t->kids[1] = r;
	t->val = v;
	t->x.state = 0;
	return t;
}

static void gen(NODEPTR p) {
	if (burm_label(p) == 0)
		std::cerr << "no cover\n";
	else {
		stmt_action(p->x.state,0);
		if (shouldCover != 0)
			dumpCover(p, 1, 0);
	}
}

using namespace llvm;

static cl::opt<std::string>
InputFilename(cl::Positional, cl::desc("<input bitcode>"), cl::init("-"));

static cl::opt<std::string>
OutputFilename("o", cl::desc("<output filename>"), cl::value_desc("filename"));

static cl::opt<int>
NumRegs("num_regs", cl::desc("<number of registers available>"), cl::init(32));

/**
 * Wrapper for tree
 * */
class TreeReference
{
public:
    TreeReference(int opcode, Tree l = nullptr, Tree r = nullptr): reference(0) {
        t = tree(opcode, l, r);
    }
    virtual ~TreeReference() {
        free(t);
    }
    void SetValue(VALUE v) {
        t->val = v;
    }
    void SetChild(int n, Tree t) {
        if (n < 2) {
            t->kids[n] = t;
        } else {
            // TODO: IMPLEMENT A WAY TO STORE MORE THAN 2 CHILDREN
            errs() << "SetChild for n=" << n << " is not implemented yet!\n";
        }
    }
    Tree GetTreeReference() { reference++; return t; }
    int GetRefCount() const { return reference; }

private:
    Tree t;
    int reference;
};

/**
 * Generate expression trees for a function
 * */
void FunctionToExprTrees(Function &func) {
    Function::BasicBlockListType &basic_blocks = func.getBasicBlockList();
    std::map<Instruction*, TreeReference*> treeMap;
    for (BasicBlock &bb : basic_blocks) {
        for (auto inst = bb.begin(); inst != bb.end(); inst++) {
            Instruction &instruction = *inst;
#if 1
            errs() << "OperandNum: " << instruction.getNumOperands() << "\t";
            instruction.print(errs()); 
            errs() << "\n";
#endif
            TreeReference t = TreeReference(instruction.getOpcode());
            int num_operands = instruction.getNumOperands();
            for (int i = 0; i < num_operands; i++) {
                Value *v = instruction.getOperand(i);
                // find the value's definition
                Instruction *def = dyn_cast<Instruction>(v);
                // // or the value is some constant
                // Constant *const = dyn_cast<Constant>(v);

                if (def) {
                    auto it = treeMap.find(def);
                    assert(it != treeMap.end() && "operands must be previously defined");
                    t.SetChild(i, it->second->GetTreeReference());
                    errs() << "FOUND DEF:\t"; it->first->print(errs()); errs() << "\n";
                }
                else  {
                    // for example: alloca i32, ...
                    errs() << "FAILED TO FIND DEF:\t"; instruction.print(errs()); errs() << "\n";
                }
            }
            // store the current tree in map
            treeMap.insert(std::pair<Instruction*, TreeReference*>(&instruction, &t));
        } // end of instruction loop
    } // end of basic block loop
}

int main(int argc, char *argv[])
{
    // parse arguments from command line
    cl::ParseCommandLineOptions(argc, argv, "llc-olive\n");

    // prepare llvm context to read bitcode file
    LLVMContext context;
    SMDiagnostic error;
    std::unique_ptr<Module> module = parseIRFile(StringRef(InputFilename.c_str()), error, context);

    // obtain a function list in module, and iterate over function
    Module::FunctionListType &function_list = module->getFunctionList();
    for (Function &func : function_list) {
        FunctionToExprTrees(func);
    }

    return 0;
}
