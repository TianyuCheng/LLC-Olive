#ifndef LLC_OLIVE_H
#define LLC_OLIVE_H

#include <iostream>
#include <sstream>
#include <fstream>
#include <cassert>
#include <vector>

#include <llvm/Support/CommandLine.h>
#include <llvm/IR/Module.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Constants.h>

#include "./FunctionState.h"
class FunctionState;

typedef struct tree {
	int op;
	struct tree *kids[3];
	VALUE val;
    int level;
    int refcnt;
    bool isReg;
    bool computed;
    X86OperandType operandType;
    llvm::Instruction *inst;
	struct { struct burm_state *state; } x;
} *NODEPTR, *Tree;

#define GET_KIDS(p)	((p)->kids)
#define PANIC printf
#define STATE_LABEL(p) ((p)->x.state)
#define SET_STATE(p,s) (p)->x.state=(s)
#define DEFAULT_COST	break
#define NO_ACTION(x)

typedef struct COST {
    int cost;
} COST;
#define COST_LESS(a,b) ((a).cost < (b).cost)

static COST COST_INFINITY = { 32767 };
static COST COST_ZERO     = { 0 };

static int _ern = 0;

static int shouldTrace = 0;
static int shouldCover = 0;

int OP_LABEL(NODEPTR p);

static void burm_trace(NODEPTR, int, COST);
Tree tree(int op, Tree l, Tree r, VALUE v = 0);
void gen(NODEPTR p, FunctionState *fstate);

/**
 * Wrapper for tree
 * */
class TreeWrapper
{
public:
    TreeWrapper(int opcode, Tree l = nullptr, Tree r = nullptr);
    virtual ~TreeWrapper();
    int GetOpCode() const { return t->op; }
    void SetValue(VALUE v) { t->val = v; }
    VALUE& GetValue() const { return t->val; }
    bool IsVirtualReg() const { return t->isReg; }
    int GetVirtualReg() const { return t->val.val.i32s; }
    void SetInst(llvm::Instruction *inst) { t->inst = inst; }
    void SetLevel(int level) { t->level = level; }
    void SetChild(int n, Tree ct);
    Tree GetChild(int n);
    Tree GetTree() { return t; }
    Tree GetTreeWrapper() { t->refcnt++; return t; }

    int GetLevel() const { return t->level; }
    int GetRefCount() const { return t->refcnt; }
    int GetNumChildren() const { return nchild; }

    void CastInt(int n, llvm::ConstantInt *cnst);
    void CastFP(int n, llvm::ConstantFP *cnst);

    void DisplayTree() const { DisplayTree(t, 0); std::cerr << "\n"; }

private:
    void DisplayTree(Tree t, int indent = 0) const;

private:
    Tree t;
    int nchild;
    std::vector<Tree> freeList;
};

enum { Ret=1, Br=2, Switch=3, IndirectBr=4, Invoke=5, Resume=6, Unreachable=7, CleanupRet=8, CatchRet=9, CatchSwitch=10 };
enum { Add=11, FAdd=12, Sub=13, FSub=14, Mul=15, FMul=16, UDiv=17, SDiv=18, FDiv=19, URem=20, SRem=21, FRem=22 };
enum { Shl=23, LShr=24, AShr=25, And=26, Or=27, Xor=28 };
enum { Alloca=29, Load=30, Store=31, GetElementPtr=32, Fence=33, AtomicCmpXchg=34, AtomicRMW=35 };
enum { Trunc=36, ZExt=37, SExt=38, FPToUI=39, FPToSI=40, UIToFP=41, SIToFP=42, FPTrunc=43, FPExt=44, PtrToInt=45, IntToPtr=46, BitCast=47, AddrSpaceCast=48 };
enum { CleanupPad=49, CatchPad=50 };
enum { ICmp=51, FCmp=52, PHI=53, Call=54, Select=55, UserOp1=56, UserOp2=57, VAArg=58, ExtractElement=59, InsertElement=60, ShuffleVector=61, ExtractValue=62, InsertValue=63, LandingPad=64 };
enum { REG=65, IMM=66, MEM=67, LABEL=68, DUMMY=0 };

#endif

