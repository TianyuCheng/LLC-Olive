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
#include <llvm/IR/Constants.h>

#include "./RegisterAllocator.h"
#include "./FunctionState.h"

typedef struct VALUE {
    union {
        int8_t      i8s;
        uint8_t     i8u;
        int16_t     i16s;
        uint16_t    i16u;
        int32_t     i32s;
        uint32_t    i32u;
        int64_t     i64s;
        uint64_t    i64u;
        float       f32;
        double      f64;
    } val;
    int bitWidth;
    bool isFP;
    bool isSigned;

    VALUE() {
        val.i64u = 0;
        bitWidth = 64;
        isFP = false;
        isSigned = false;
    }

    VALUE(int8_t i)   { bitWidth =  4; isSigned = true;  isFP = false; val.i8s = i;  }
    VALUE(int16_t i)  { bitWidth = 16; isSigned = true;  isFP = false; val.i16s = i; }
    VALUE(int32_t i)  { bitWidth = 32; isSigned = true;  isFP = false; val.i32s = i; }
    VALUE(int64_t i)  { bitWidth = 64; isSigned = true;  isFP = false; val.i64s = i; }
    VALUE(uint8_t i)  { bitWidth =  8; isSigned = false; isFP = false; val.i8u = i;  }
    VALUE(uint16_t i) { bitWidth = 16; isSigned = false; isFP = false; val.i16u = i; }
    VALUE(uint32_t i) { bitWidth = 32; isSigned = false; isFP = false; val.i32u = i; }
    VALUE(uint64_t i) { bitWidth = 64; isSigned = false; isFP = false; val.i64u = i; }
    VALUE(float f)    { bitWidth = 32;                   isFP = true ; val.f32 = f;  }
    VALUE(double f)   { bitWidth = 64;                   isFP = true ; val.f32 = f;  }
} VALUE;

typedef struct tree {
	int op;
	struct tree *kids[2];
	VALUE val;
    int level;
    int refcnt;
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

int OP_LABEL(NODEPTR p) {
    if (p) return p->op;
    std::cerr << "NODEPTR is null for OP_LABEL" << std::endl;
    exit(EXIT_FAILURE);
}

static void burm_trace(NODEPTR, int, COST);
static std::string MRI2String(int type, Tree t);

static const char* registers[] = {
    "eax", "ebx", "ecx", "edx",
    "edi", "esi", "ebp", "esp",
    "r1d", "r2d", "r3d", "r4d",
    "r5d", "r6d", "r7d", "r8d",
    "r9d", "r10d", "r11d", "r12d"
};
static int NUM_REGS = 20;
