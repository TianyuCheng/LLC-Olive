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
                else if (Function *func = dyn_cast<Function>(v)) {
                    t->SetFuncName(v->getName().str());
                }
                // ... There are many kinds of constant, right now we do not deal with them ...
                else {
                    // this is bad and probably needs to terminate the execution
                    errs() << "NOT IMPLEMENTED OTHER CONST TYPES:\t"; instruction.print(errs()); errs() << "\n";
                    errs() << "OPERAND: "; v->print(errs()); errs() << "\n";
#if 0               // we might need to handle undef value some time later
                    if (UndefValue *undef = dyn_cast<UndefValue>(v)) {
                        errs() << "OPERAND IS AN UNDEF VALUE\n";
                    }
#endif
                    // exit(EXIT_FAILURE);
                }
            }
            else if (Instruction *def = dyn_cast<Instruction>(v)) {
                // for regular operands
                // check if we can find operand's definition
                Tree *wrapper = fstate.FindFromTreeMap(def);
                assert(wrapper && "operands must be previously defined");
                assert (wrapper != t);
                t->AddChild(wrapper->GetTreeRef());      // automatically increase the refcnt
            }
            else if (BasicBlock *block = dyn_cast<BasicBlock>(v)) {
                // for branches
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
            else if (Argument *arg = dyn_cast<Argument>(v)) {
                // for argument operands
                Tree *wrapper = fstate.FindFromTreeMap(arg);
                assert(wrapper && "arguments must be previously defined");
                assert (wrapper != t);
                t->AddChild(wrapper->GetTreeRef());      // automatically increase the refcnt
            }
            else {
                // TODO: write code to handle these situations
                errs() << "Unhandle-able instruction operand! Quit\n";
                errs() << "Operand: "; v->print(errs()); errs() << "\n";
                exit(EXIT_FAILURE);
            }
        } //end of operand loop

        // HACK for making BranchInst all 3 address code
        // ---------------------------------------------
        switch (instruction.getOpcode()) {
            case Br:
                if (t->GetNumKids() == 1) {
                    // first fill the branch inst with dummy node
                    t->AddChild(new Tree(DUMMY));
                    t->AddChild(new Tree(DUMMY));
                }
                break;
            case Call:
                t->KidsAsArguments();
                break;
        }
        // ---------------------------------------------

        // store the current tree in map
        fstate.AddToTreeMap(&instruction, t);
        treeList.push_back(t);
    } // end of instruction loop in a basic block
}

void get_opr_counter (Function &func, &std::map<Instruction*, int>& inst_opr_counter, std::map<BasicBlock*, pair<int,int>>& bb_opr_counter) {
    Function::BasicBlockListType &basic_blocks = func.getBasicBlockList();
    int counter = 0;
    for (auto bb = basic_blocks.begin(); bb != basic_blocks.end(); bb++) {
        int from = counter;
        for (auto inst=bb->begin(); inst!=bb->end(); inst++) {
            unsigned opcode = inst->getOpcode();
            if (opcode != PHI) { // only assign number to non-phi instruction 
                inst_opr_counter.insert(std::make_pair(&(*inst), counter));
                counter += 2;
            }
        }
        int to = counter;
        // left inclusive, right exclusive
        bb_opr_counter.insert(std::make_pair(&(*bb)), std::make_pair(from,to));
    }
    return ;
}

void FunctionToIntervals (Function &func) {
    std::map<BasicBlock*, std::set<int>*> livein;
    std::map<Value*, int> v2vr_map;
    std::map<int, Interval*> all_intervals;
    std::map<Instruction*, int> inst_opr_counter;
    std::map<BasicBlock*, int> bb_opr_counter;
    get_opr_counter(func, inst_opr_counter, bb_opr_counter);
    int vr_count = 0;
    Function::BasicBlockListType &basic_blocks = func.getBasicBlockList();
    // FOR EACH block in reverse order
    for (auto bb = basic_blocks.rbegin(); bb != basic_blocks.rend(); bb++) {
        std::set<int> live;
        BasicBlock *block = &(*bb);
        // 1. get union of successor.livein FOR EACH successor
        TerminatorInst* termInst = bb->getTerminator();
        int numSuccessors = termInst->getNumSuccessors();
        for (int i = 0; i < numSuccessors; i++) {
            BasicBlock* succ = termInst->getSuccessor(i);
            auto it = livein.find(succ);
            assert((it != livein.end()) && "find succ fails at 1.");
            std::set<int>* succ_livein = it->second;
            for (auto it=succ_livein->begin(); it!=succ_livein->end(); it++)
                live.insert(*it);
        }
        // 2. FOR EACH PHI function of 
        for (int i = 0; i < numSuccessors; i++) {
            BasicBlock* succ = termInst->getSuccessor(i);
            for (auto inst=succ->begin(); inst!=succ->end(); inst++) {
                unsigned opcode = inst->getOpcode();
                if (opcode != PHI) continue;
                int num_operands = inst->getNumOperands();
                for (int j = 0; j < num_operands; j++) {
                    // TODO: particularly consider constant type
                    Value *v = inst->getOperand(j);
                    if (v2vr_map.find(v) == v2vr_map.end()) 
                        // create an new virtual register number and assign to it
                        live.insert(vr_count++);  
                    else live.insert(v2vr_map[v]);
                }
            }
        }
        // 3. add ranges FOR EACH opd (virtual register) in live
        pair<int,int> opr_pair;
        if (bb_opr_counter.find(block) == bb_opr_counter.end())
            assert(false && "BasicBlock not found in bb_opr_counter!");
        else 
            opr_pair = bb_opr_counter[block];
        int bb_from = opr_pair.first, bb_to = opr_pair.second;
        for (int opd : live) {
            if (all_intervals.find(opd) == all_intervals.end()) {
                LiveRange lr (bb_from, bb_to);
                all_intervals[opd] = &lr; 
            } else {
                all_intervals[opd].addRange(bbfrom, bbto);
            }
        }
        // 4. 
        for (auto inst=bb->rbegin(); inst!=bb->rend(); inst++) {
            unsigned opcode = inst->getOpcode();
            if (opcode == PHI) continue; // FIXME: check if non-phi instruction
            // FOR EACH output operand of inst
            if (v2vr_map.find(inst) == v2vr_map.end()) {
                assert(false && "inst is not found in 4.");
            } else {
                int opid = -1; // TODO: 
                all_intervals[v2vr_map[inst]].setFrom(opid);
                live.erase(v2vr_map[inst]);
            }
            live.erase(v2vr_map[inst]);
            // FOR EACH input operand of inst
            int num_operands = inst->getNumOperands();
            for (int j = 0; j < num_operands; j++) {
                Value *v = inst->getOperand(j);
                if (v2vr_map.find(v) == v2vr_map.end()) {
                    assert(false && "v is not found in 4.");
                } else {
                    int bfrom = -1, opid = -1; // TODO:
                    all_intervals[v2vr_map[v]].addRange(bfrom, opid);
                    live.insert(v2vr_map[v]);
                }
            }
        }
        // 5. 
        for (auto inst=bb->begin(); inst!=bb->end(); inst++) {
            unsigned opcode = inst->getOpcode();
            if (opcode != PHI) continue;
            if (v2vr_map.find(inst) == v2vr_map.end()) continue;
            else live.erase(v2vr_map[v]);
        }
        // 6. if b is loop header
        if ( block ==  ) {
            ;
            for (int opd : live) {
                int bfrom = -1, loopEndTo = -1;
                all_intervals[opd].addRange(bfrom, loopEndTo);
            }
        }
        // 7. update back to livein
        livein.insert(std::pair<BasicBlock*, std::set<int>*>(block, &live));
    }
}

/**
 * Generate assembly for a single function
 * */
void FunctionToAssembly(Function &func) {

    // prepare a function state container
    // to store function information, such as
    // local variables, free registers, etc
    FunctionState fstate(func.getName(), NumRegs);

    // pass in arguments
    Function::ArgumentListType &arguments = func.getArgumentList();
    for (Argument &arg : arguments)
        fstate.CreateArgument(&arg);

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
            fstate.GenerateLabelStmt(wrapper);
        }
        BasicBlockToExprTrees(fstate, treeList, bb);

        // iterate through tree list for each individual instruction tree
        // replace the complicated/common tree expression with registers
        for (int i = size; i < treeList.size(); i++) {
            Tree *t = treeList[i];
            Instruction::getOpcodeName(t->GetOpCode());
            t->GetLevel();
            t->GetRefCount();
#if VERBOSE > 2
            errs() << Instruction::getOpcodeName(t->GetOpCode()) << "\tLEVEL:\t" << t->GetLevel() << "\tRefCount:\t" << t->GetRefCount() << "\n";
            errs() << "NumOperands: " << t->GetNumKids() << "\n";
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
        FunctionToIntervals(func);
        FunctionToAssembly(func);
    }

    return 0;
}
