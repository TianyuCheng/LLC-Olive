#define VERBOSE  0
#define DEBUG    0
#define SSA_REGISTER_ALLOCATOR 0

// global variables

static int labelID = 0;
static int functionID = 0;

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
#if 0
                    errs() << "NOT IMPLEMENTED CONST EXPR\n";
                    exit(EXIT_FAILURE);
#endif
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

void get_opr_counter (Function &func, std::map<Instruction*, int>& inst_opr_counter, std::map<BasicBlock*, std::pair<int,int>>& bb_opr_counter) {
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
        bb_opr_counter.insert(std::make_pair(&(*bb), std::make_pair(from,to)));
    }
    return ;
}

void BuildIntervals (Function &func, std::map<int, Interval*> &all_intervals, std::map<int, std::vector<int>*> &use_contexts) {
    // Preliminary: local variables
    std::map<BasicBlock*, std::set<int>*> livein;
    std::map<Value*, int> v2vr_map;
    std::map<Instruction*, int> inst_opr_counter;
    std::map<BasicBlock*, std::pair<int,int>> bb_opr_counter;
    // LoopInfo& loopinfo = getAnalysis<LoopInfoWrapperPass>().getLoopInfo();
    int vr_count = 0;
    // Preliminary: get operation numbers and all basic blocks within functions
    get_opr_counter(func, inst_opr_counter, bb_opr_counter);
    Function::BasicBlockListType &basic_blocks = func.getBasicBlockList();
    // FOR EACH block in reverse order
    for (auto bb = basic_blocks.rbegin(); bb != basic_blocks.rend(); bb++) {
        std::set<int> live;
        BasicBlock *block = &(*bb);
       // std::cout << "block:" << bb_opr_counter[block].first << ", " << bb_opr_counter[block].second << std::endl;
        // 1. get union of successor.livein FOR EACH successor
      //  std::cout << "Now step 2" << std::endl; 
        TerminatorInst* termInst = bb->getTerminator();
        int numSuccessors = termInst->getNumSuccessors();
        for (int i = 0; i < numSuccessors; i++) {
            BasicBlock* succ = termInst->getSuccessor(i);
         //   std::cout << "succ:" << bb_opr_counter[succ].first << ", " << bb_opr_counter[succ].second << std::endl;
            if (livein.find(succ) == livein.end()) { ;
               // std::cout << "it is succ not processed yet. so insert empty. " << std::endl;
            } else {
                std::set<int>* succ_livein = livein[succ];
                for (auto it=succ_livein->begin(); it!=succ_livein->end(); it++)
                    live.insert(*it);
            }
        }
        // 2. FOR EACH PHI function of 
     // std::cout << "Now step 2" << std::endl; 
        for (int i = 0; i < numSuccessors; i++) {
            BasicBlock* succ = termInst->getSuccessor(i);
            for (auto inst=succ->begin(); inst!=succ->end(); inst++) {
                unsigned opcode = inst->getOpcode();
                if (opcode != PHI) continue;
                int num_operands = inst->getNumOperands();
                for (int j = 0; j < num_operands; j++) {
                    // TODO: particularly consider constant type
                    Value *v = inst->getOperand(j);
                    if (v2vr_map.find(v) == v2vr_map.end()) {
                        // create an new virtual register number and assign to it
                        live.insert(vr_count);
                        v2vr_map.insert(std::make_pair(v, vr_count++));
                    } else live.insert(v2vr_map[v]);
                }
            }
        }
        // 3. add ranges FOR EACH opd (virtual register) in live
      // std::cout << "Now step 3" << std::endl; 
        std::pair<int,int> opr_pair;
        if (bb_opr_counter.find(block) == bb_opr_counter.end())
            assert(false && "BasicBlock not found in bb_opr_counter!");
        else 
            opr_pair = bb_opr_counter[block];
        int bb_from = opr_pair.first, bb_to = opr_pair.second;
        for (int opd : live) {
            if (all_intervals.find(opd) == all_intervals.end()) {
                Interval* new_interval = new Interval (bb_from, bb_to);
                all_intervals.insert(std::make_pair(opd, new_interval)); 
            } else {
          //      std::cout << opd << " addRange(" << bb_from << "," << bb_to << ")" << std::endl;
                all_intervals[opd]->addRange(bb_from, bb_to);
            }
        }
        // 4. 
     //  std::cout << "Now step 4" << std::endl; 
        for (auto it=bb->rbegin(); it!=bb->rend(); it++) {
            Instruction* inst = &(*it);
            unsigned opcode = inst->getOpcode();
            if (opcode == PHI) continue; // FIXME: check if non-phi instruction
            int opid; 
            if (inst_opr_counter.find(inst) == inst_opr_counter.end())
                assert(false && "inst not found in inst_opr_counter!");
            else opid = inst_opr_counter[inst];
            // FOR EACH output operand of inst
            if (inst->getOpcode() == PHI) continue;
           // std::cout << "Instruction: " << opid << std::endl; 
            Value* v = (Value *) (&(* inst));
            // std::cout << "here:1 " << std::endl; 
            if (v2vr_map.find(v) == v2vr_map.end()) 
                v2vr_map.insert(std::make_pair(v, vr_count++));
            // std::cout << "here:2-->" << v2vr_map[v] << std::endl; 
            int inst_opd = v2vr_map[v];
            if (all_intervals.find(inst_opd) == all_intervals.end()) { 
                Interval* new_interval = new Interval (opid, bb_to);
                all_intervals.insert(std::make_pair(inst_opd, new_interval));
            } else 
                all_intervals[inst_opd]->setFrom(opid, bb_to);
            live.erase(inst_opd);
            // FOR EACH input operand of inst
            //  std::cout << "FOR EACH input operand of inst: " << std::endl; 
            int num_operands = inst->getNumOperands();
            for (int j = 0; j < num_operands; j++) {
                v = inst->getOperand(j);
                int v_opd;
               // std::cout << "AAAAAAAAAA: " << std::endl; 
                if (v2vr_map.find(v) == v2vr_map.end()) 
                    v2vr_map.insert(std::make_pair(v, vr_count++));
                v_opd = v2vr_map[v];
              //  std::cout << "v_opd: "<< v_opd << std::endl; 
                if (all_intervals.find(v_opd) == all_intervals.end()) {
              //  std::cout << "new interval: " << std::endl; 
                    Interval* new_interval = new Interval (bb_from, opid);
                    all_intervals.insert(std::make_pair(v_opd, new_interval));
                } else {
                  //  std::cout << "addRange: " << bb_from << ", " << opid << std::endl; 
                   // std::cout << "prev_lr: " << all_intervals[v_opd]->liveranges[0].startpoint << ", "
                  //      << all_intervals[v_opd]->liveranges[0].endpoint << std::endl; 
            //    std::cout << v_opd << " addRange(" << bb_from << "," << opid << ")" << std::endl;
                    all_intervals[v_opd]->addRange(bb_from, opid);
                }
               //  std::cout << "CCCCCCCCC: " << std::endl; 
                live.insert(v_opd);
                int v_opr_number;
                if (inst_opr_counter.find(inst) == inst_opr_counter.end())
                    assert(false && "v cannot be found in inst_opr_counter in 4.");
                else 
                    v_opr_number = inst_opr_counter[inst];
                //  std::cout << "DDDDDDDDDDDD: " << std::endl; 
                if (use_contexts.find(v_opd) == use_contexts.end()) {
                    //    std::cout << "sssssssssssssss: " << std::endl; 
                    std::vector<int>* new_use = new std::vector<int>();
                    new_use->push_back(v_opr_number);
                    use_contexts.insert(std::make_pair(v_opd, new_use)); 
                } else { 
                    //   std::cout << "eeeeeeeeeee: " << std::endl; 
                    use_contexts[v_opd]->push_back(v_opr_number);
                }
                //  std::cout << "EEEEEEEEE: " << std::endl; 
            }
        }
        // 5. 
      // std::cout << "Now step 5" << std::endl; 
        for (auto inst=bb->begin(); inst!=bb->end(); inst++) {
            unsigned opcode = inst->getOpcode();
            if (opcode != PHI) continue;
             //std::cout << "qqqqqqqqqqqqq: " << std::endl; 
            Value* v = (Value *) (&(* inst));
            // std::cout << "ppppppppp: " << std::endl; 
            if (v2vr_map.find(v) == v2vr_map.end()) {
                continue;
               //  std::cout << "ppppppp111111111pp: " << std::endl; 
            } else {
                // std::cout << "ppppppp222222222pp: " << std::endl; 
                live.erase(v2vr_map[v]);
            }
        }
        // 6. if b is loop header
        /*
        if (loopinfo.isLoopHeader(block)) {
            Loop* cur_loop = loopinfo.getLoopFor(block);
            BasicBlock* loopEnd = &(*(cur_loop->rbegin()));
            int loopEndTo;
            if (bb_opr_counter.find(loopEnd) == bb_opr_counter.end())
                assert(false && "BasicBlock LoopEnd not found in bb_opr_counter!");
            else 
                loopEndTo = bb_opr_counter[loopEnd].second;           
            for (int opd : live) 
                all_intervals[opd]->addRange(bb_from, loopEndTo);
        }
        */
        // 7. update back to livein
        // livein.insert(std::pair<BasicBlock*, std::set<int>*>(block, &live));
        // std::cout << "EEEEEEEEE: " << std::endl; 
        livein.insert(std::make_pair(block, &live));
        // std::cout << "sddsdssdsd: " << std::endl; 
    }
    // post processing: reverse all use_contexts[opd]
      //  std::cout << "Now postprocessing" << std::endl; 
    for (auto it=use_contexts.begin(); it != use_contexts.end(); it++) 
        std::reverse(it->second->begin(), it->second->end());
      //  std::cout << "Now postprocessing end" << std::endl; 
}

/**
 * Generate assembly for a single function
 * */
void MakeAssembly(Function &func, /*RegisterAllocator &ra,*/ std::ostream &out) {

    // prepare a function state container
    // to store function information, such as
    // local variables, free registers, etc
    FunctionState fstate(func.getName(), NumRegs, functionID, labelID);

    // pass in arguments
    Function::ArgumentListType &arguments = func.getArgumentList();
    for (Argument &arg : arguments)
        fstate.CreateArgument(&arg);

    Function::BasicBlockListType &basic_blocks = func.getBasicBlockList();
    // === First pass: Collect basic block info, which basic block will need label
    for (BasicBlock &bb : basic_blocks) {
        for (auto inst = bb.begin(); inst != bb.end(); inst++) {
            int num_operands = inst->getNumOperands();
            // check basic block being referenced
            for (int i = 0; i < num_operands; i++) {
                Value *v = inst->getOperand(i);
                if (!dyn_cast<Instruction>(v))
                    if (BasicBlock *block = dyn_cast<BasicBlock>(v))
                        fstate.CreateLabel(block);
            } // end of operand loop

#if 0
            // check phi instruction
            Instruction instruction = *inst;
            if (PhiNode::classof(&instruction)) {
                for (int i = 0; i < num_operands; i++) {
                    Value *v = inst->getOperand(i);
                    if (Instruction *def = dyn_cast<Instruction>(v)) {
                        BasicBlock *parentBlock = def->getParent();
                        Instruction *ins = new Instruction(v, , parentBlock);
                    }
                }
            }
#endif
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
        for (unsigned i = size; i < treeList.size(); i++) {
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
    fstate.PrintAssembly(out /*, ra*/);

    // clean up
    // for (Tree *t : treeList) delete t;

    functionID = fstate.GetFunctionID() + 1;
    labelID = fstate.GetLabelID() + 1;
}

int main(int argc, char *argv[])
{
    // parse arguments from command line
    cl::ParseCommandLineOptions(argc, argv, "llc-olive\n");

    // prepare llvm context to read bitcode file
    LLVMContext context;
    SMDiagnostic error;
    std::unique_ptr<Module> module = parseIRFile(StringRef(InputFilename.c_str()), error, context);

#if VERBOSE
    errs() << "Num-Regs: " << NumRegs << "\n";
#endif

    std::ofstream assemblyOut;
    assemblyOut.open(OutputFilename.c_str());
    assert(assemblyOut.good());
    assemblyOut << "\t.file\t\"" << InputFilename.c_str() << "\"" << std::endl;
    assemblyOut << "\t.text" << std::endl;

    // obtain a function list in module, and iterate over function

    // check if this bitcode file contains a main function or not
    Module::FunctionListType &function_list = module->getFunctionList();

    // iterate through all functions to generate code for each function
    for (Function &func : function_list) {
        // we cannot handle intrinsic function, but we need it to compile for array
        if (func.isDeclaration() && !func.isIntrinsic()) continue;
#if SSA_REGISTER_ALLOCATOR
        std::cout << "#################################################" << std::endl;
        std::cout << "Start build lifetime intervals.." << std::endl;
        std::cout << "#################################################" << std::endl;
        std::map<int, Interval*> all_intervals;
        std::map<int, std::vector<int>*> use_contexts;
        BuildIntervals(func, all_intervals, use_contexts);
        for (auto it=all_intervals.begin(); it!=all_intervals.end(); it++) {
            std::cout << it->first << ":" 
                << "start=" << it->second->liveranges[0].startpoint
                << ", end="  << it->second->liveranges.rbegin()->endpoint 
                << ", size=" << it->second->liveranges.size()
                << ", liveranges:";
            for (auto x = it->second->liveranges.begin(); x != it->second->liveranges.end(); x++) {
                std::cout  << " (" << x->startpoint << "," << x->endpoint << ") " ;
            }
            std::cout << std::endl;
        }
        std::cout << "------------------use_contexts----------------------" << std::endl;

        for (auto it=use_contexts.begin(); it!=use_contexts.end(); it++) {
            std::cout << "virReg=" << it->first << ", freq=" << it->second->size() << ", use at:";
            for (int x : (*(it->second))) {
                std::cout  << " " << x;
            }
            std::cout << std::endl;
        }
        std::cout << "#################################################" << std::endl;
        std::cout << "Start Linear Scan Allocation.." << std::endl;
        std::cout << "#################################################" << std::endl;
        RegisterAllocator ra (NumRegs);
        ra.set_all_intervals(all_intervals);
        ra.set_use_contexts(use_contexts);
        ra.linearScanSSA();
        std::cout << "#################################################" << std::endl;
        std::cout << "Start Generate Assembly Code.." << std::endl;
        std::cout << "#################################################" << std::endl;
#endif

#if DEBUG
        MakeAssembly(func, /*ra,*/ std::cerr);
        std::cerr << std::endl;               // separator line
#else
        MakeAssembly(func, /*ra,*/ assemblyOut);
        assemblyOut << std::endl;               // separator line
#endif
    }

    return 0;
}
