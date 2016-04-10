#ifndef __OLIVE_HEADER_INCLUDED__
#define __OLIVE_HEADER_INCLUDED__
#include <assert.h>
#include <iostream>
#include <stdlib.h>
#include <string>
#include "llc_olive_helper.h"
#define DUMMY 0
#define Ret 1
#define Br 2
#define Switch 3
#define IndirectBr 4
#define Invoke 5
#define Resume 6
#define Unreachable 7
#define CleanupRet 8
#define CatchRet 9
#define CatchSwitch 10
#define Add 11
#define FAdd 12
#define Sub 13
#define FSub 14
#define Mul 15
#define FMul 16
#define UDiv 17
#define SDiv 18
#define FDiv 19
#define URem 20
#define SRem 21
#define FRem 22
#define Shl 23
#define LShr 24
#define AShr 25
#define And 26
#define Or 27
#define Xor 28
#define Alloca 29
#define Load 30
#define Store 31
#define GetElementPtr 32
#define Fence 33
#define AtomicCmpXchg 34
#define AtomicRMW 35
#define Trunc 36
#define ZExt 37
#define SExt 38
#define FPToUI 39
#define FPToSI 40
#define UIToFP 41
#define SIToFP 42
#define FPTrunc 43
#define FPExt 44
#define PtrToInt 45
#define IntToPtr 46
#define BitCast 47
#define AddrSpaceCast 48
#define CleanupPad 49
#define CatchPad 50
#define ICmp 51
#define FCmp 52
#define PHI 53
#define Call 54
#define Select 55
#define UserOp1 56
#define UserOp2 57
#define VAArg 58
#define ExtractElement 59
#define InsertElement 60
#define ShuffleVector 61
#define ExtractValue 62
#define InsertValue 63
#define LandingPad 64
#define REG 65
#define IMM 66
#define MEM 67
#define LABEL 68
#define ARGS 69
#define NOARGS 70
#define PTR 71
#define GlobalValue 72

struct burm_state {
  int op;
  NODEPTR node;
  struct burm_state **kids;
  COST cost[12];
  struct {
    unsigned burm_stmt:4;
    unsigned burm_value:2;
    unsigned burm_ri:2;
    unsigned burm_rm:2;
    unsigned burm_reg:5;
    unsigned burm_imm:2;
    unsigned burm_mem:3;
    unsigned burm_ptr:2;
    unsigned burm_cond:2;
    unsigned burm_label:1;
    unsigned burm_args:2;
  } rule;
};


struct burm_state *burm_label(NODEPTR);
struct burm_state *burm_label1(NODEPTR);

void dumpCover(NODEPTR,int,int);

#endif
