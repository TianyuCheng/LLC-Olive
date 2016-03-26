#include <iostream>
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

    VALUE() { val.i64u = 0; } 

    VALUE(int8_t i)   { val.i8s = i; } 
    VALUE(int16_t i)  { val.i16s = i; } 
    VALUE(int32_t i)  { val.i32s = i; } 
    VALUE(int64_t i)  { val.i64s = i; } 
    VALUE(uint8_t i)  { val.i8u = i; } 
    VALUE(uint16_t i) { val.i16u = i; } 
    VALUE(uint32_t i) { val.i32u = i; } 
    VALUE(uint64_t i) { val.i64u = i; } 
    VALUE(float f)    { val.f32 = f; } 
    VALUE(double f)   { val.f32 = f; } 
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
static int shouldCover = 1;

int OP_LABEL(NODEPTR p) {
	switch (p->op) {
	default:     return p->op - 1;      // -1 for offset in array
	}
}

static void burm_trace(NODEPTR, int, COST);
