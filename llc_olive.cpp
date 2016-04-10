#include "llc_olive.h"
#define burm_stmt_NT 1
#define burm_value_NT 2
#define burm_ri_NT 3
#define burm_rm_NT 4
#define burm_reg_NT 5
#define burm_imm_NT 6
#define burm_mem_NT 7
#define burm_ptr_NT 8
#define burm_cond_NT 9
#define burm_label_NT 10
#define burm_args_NT 11
extern int burm_max_nt;


void stmt_action(struct burm_state *_s, 

FUNCTION_STATE fstate);


void value_action(struct burm_state *_s, 

FUNCTION_STATE fstate);


void ri_action(struct burm_state *_s, 

FUNCTION_STATE fstate);


void rm_action(struct burm_state *_s, 

FUNCTION_STATE fstate);


void reg_action(struct burm_state *_s, 

FUNCTION_STATE fstate);


void imm_action(struct burm_state *_s, 

FUNCTION_STATE fstate);


void mem_action(struct burm_state *_s, 

FUNCTION_STATE fstate);


void ptr_action(struct burm_state *_s, 

FUNCTION_STATE fstate);


void cond_action(struct burm_state *_s, 

FUNCTION_STATE fstate);


void label_action(struct burm_state *_s, 

FUNCTION_STATE fstate);


void args_action(struct burm_state *_s, 

FUNCTION_STATE fstate, int n);
#ifndef ALLOC
#define ALLOC(n) malloc(n)
#endif

#ifndef burm_assert
#define burm_assert(x,y) if (!(x)) {  y; abort(); }
#endif

static NODEPTR burm_np;

#define burm_stmt_NT 1
#define burm_value_NT 2
#define burm_ri_NT 3
#define burm_rm_NT 4
#define burm_reg_NT 5
#define burm_imm_NT 6
#define burm_mem_NT 7
#define burm_ptr_NT 8
#define burm_cond_NT 9
#define burm_label_NT 10
#define burm_args_NT 11
extern int burm_max_nt;
int burm_max_nt = 11;

std::string burm_ntname[] = {
  "",
  "stmt",
  "value",
  "ri",
  "rm",
  "reg",
  "imm",
  "mem",
  "ptr",
  "cond",
  "label",
  "args",
  ""
};

static short burm_nts_0[] = { burm_reg_NT, 0 };
static short burm_nts_1[] = { burm_mem_NT, 0 };
static short burm_nts_2[] = { burm_imm_NT, 0 };
static short burm_nts_3[] = { 0 };
static short burm_nts_4[] = { burm_ptr_NT, 0 };
static short burm_nts_5[] = { burm_imm_NT, burm_mem_NT, 0 };
static short burm_nts_6[] = { burm_reg_NT, burm_mem_NT, 0 };
static short burm_nts_7[] = { burm_mem_NT, burm_mem_NT, 0 };
static short burm_nts_8[] = { burm_reg_NT, burm_ri_NT, 0 };
static short burm_nts_9[] = { burm_ri_NT, burm_reg_NT, 0 };
static short burm_nts_10[] = { burm_rm_NT, burm_rm_NT, 0 };
static short burm_nts_11[] = { burm_rm_NT, burm_imm_NT, 0 };
static short burm_nts_12[] = { burm_cond_NT, 0 };
static short burm_nts_13[] = { burm_imm_NT, burm_imm_NT, 0 };
static short burm_nts_14[] = { burm_reg_NT, burm_imm_NT, 0 };
static short burm_nts_15[] = { burm_reg_NT, burm_reg_NT, 0 };
static short burm_nts_16[] = { burm_label_NT, 0 };
static short burm_nts_17[] = { burm_cond_NT, burm_label_NT, burm_label_NT, 0 };
static short burm_nts_18[] = { burm_args_NT, 0 };
static short burm_nts_19[] = { burm_value_NT, burm_args_NT, 0 };
static short burm_nts_20[] = { burm_ri_NT, burm_ri_NT, burm_imm_NT, 0 };
static short burm_nts_21[] = { burm_mem_NT, burm_ri_NT, burm_imm_NT, 0 };

short *burm_nts[] = {
  burm_nts_0,  /* 0 */
  burm_nts_1,  /* 1 */
  burm_nts_0,  /* 2 */
  burm_nts_2,  /* 3 */
  burm_nts_1,  /* 4 */
  burm_nts_0,  /* 5 */
  burm_nts_2,  /* 6 */
  burm_nts_0,  /* 7 */
  burm_nts_1,  /* 8 */
  burm_nts_3,  /* 9 */
  burm_nts_3,  /* 10 */
  burm_nts_3,  /* 11 */
  burm_nts_3,  /* 12 */
  burm_nts_4,  /* 13 */
  burm_nts_3,  /* 14 */
  burm_nts_2,  /* 15 */
  burm_nts_0,  /* 16 */
  burm_nts_2,  /* 17 */
  burm_nts_0,  /* 18 */
  burm_nts_2,  /* 19 */
  burm_nts_1,  /* 20 */
  burm_nts_5,  /* 21 */
  burm_nts_6,  /* 22 */
  burm_nts_7,  /* 23 */
  burm_nts_8,  /* 24 */
  burm_nts_9,  /* 25 */
  burm_nts_8,  /* 26 */
  burm_nts_8,  /* 27 */
  burm_nts_9,  /* 28 */
  burm_nts_10,  /* 29 */
  burm_nts_10,  /* 30 */
  burm_nts_11,  /* 31 */
  burm_nts_11,  /* 32 */
  burm_nts_12,  /* 33 */
  burm_nts_13,  /* 34 */
  burm_nts_14,  /* 35 */
  burm_nts_15,  /* 36 */
  burm_nts_16,  /* 37 */
  burm_nts_17,  /* 38 */
  burm_nts_18,  /* 39 */
  burm_nts_3,  /* 40 */
  burm_nts_19,  /* 41 */
  burm_nts_0,  /* 42 */
  burm_nts_0,  /* 43 */
  burm_nts_1,  /* 44 */
  burm_nts_20,  /* 45 */
  burm_nts_21,  /* 46 */
};

char burm_arity[] = {
  0,  /* 0=DUMMY */
  1,  /* 1=Ret */
  3,  /* 2=Br */
  0,  /* 3=Switch */
  0,  /* 4=IndirectBr */
  0,  /* 5=Invoke */
  0,  /* 6=Resume */
  0,  /* 7=Unreachable */
  0,  /* 8=CleanupRet */
  0,  /* 9=CatchRet */
  0,  /* 10=CatchSwitch */
  2,  /* 11=Add */
  0,  /* 12=FAdd */
  2,  /* 13=Sub */
  0,  /* 14=FSub */
  2,  /* 15=Mul */
  0,  /* 16=FMul */
  2,  /* 17=UDiv */
  2,  /* 18=SDiv */
  0,  /* 19=FDiv */
  0,  /* 20=URem */
  0,  /* 21=SRem */
  0,  /* 22=FRem */
  0,  /* 23=Shl */
  0,  /* 24=LShr */
  0,  /* 25=AShr */
  0,  /* 26=And */
  0,  /* 27=Or */
  0,  /* 28=Xor */
  1,  /* 29=Alloca */
  1,  /* 30=Load */
  2,  /* 31=Store */
  3,  /* 32=GetElementPtr */
  0,  /* 33=Fence */
  0,  /* 34=AtomicCmpXchg */
  0,  /* 35=AtomicRMW */
  1,  /* 36=Trunc */
  0,  /* 37=ZExt */
  0,  /* 38=SExt */
  0,  /* 39=FPToUI */
  0,  /* 40=FPToSI */
  0,  /* 41=UIToFP */
  0,  /* 42=SIToFP */
  0,  /* 43=FPTrunc */
  0,  /* 44=FPExt */
  0,  /* 45=PtrToInt */
  0,  /* 46=IntToPtr */
  1,  /* 47=BitCast */
  0,  /* 48=AddrSpaceCast */
  0,  /* 49=CleanupPad */
  0,  /* 50=CatchPad */
  2,  /* 51=ICmp */
  0,  /* 52=FCmp */
  0,  /* 53=PHI */
  1,  /* 54=Call */
  0,  /* 55=Select */
  0,  /* 56=UserOp1 */
  0,  /* 57=UserOp2 */
  0,  /* 58=VAArg */
  0,  /* 59=ExtractElement */
  0,  /* 60=InsertElement */
  0,  /* 61=ShuffleVector */
  0,  /* 62=ExtractValue */
  0,  /* 63=InsertValue */
  0,  /* 64=LandingPad */
  0,  /* 65=REG */
  0,  /* 66=IMM */
  0,  /* 67=MEM */
  0,  /* 68=LABEL */
  2,  /* 69=ARGS */
  0,  /* 70=NOARGS */
  0,  /* 71=PTR */
};

std::string burm_opname[] = {
  /* 0 */  "DUMMY",
  /* 1 */  "Ret",
  /* 2 */  "Br",
  /* 3 */  "Switch",
  /* 4 */  "IndirectBr",
  /* 5 */  "Invoke",
  /* 6 */  "Resume",
  /* 7 */  "Unreachable",
  /* 8 */  "CleanupRet",
  /* 9 */  "CatchRet",
  /* 10 */  "CatchSwitch",
  /* 11 */  "Add",
  /* 12 */  "FAdd",
  /* 13 */  "Sub",
  /* 14 */  "FSub",
  /* 15 */  "Mul",
  /* 16 */  "FMul",
  /* 17 */  "UDiv",
  /* 18 */  "SDiv",
  /* 19 */  "FDiv",
  /* 20 */  "URem",
  /* 21 */  "SRem",
  /* 22 */  "FRem",
  /* 23 */  "Shl",
  /* 24 */  "LShr",
  /* 25 */  "AShr",
  /* 26 */  "And",
  /* 27 */  "Or",
  /* 28 */  "Xor",
  /* 29 */  "Alloca",
  /* 30 */  "Load",
  /* 31 */  "Store",
  /* 32 */  "GetElementPtr",
  /* 33 */  "Fence",
  /* 34 */  "AtomicCmpXchg",
  /* 35 */  "AtomicRMW",
  /* 36 */  "Trunc",
  /* 37 */  "ZExt",
  /* 38 */  "SExt",
  /* 39 */  "FPToUI",
  /* 40 */  "FPToSI",
  /* 41 */  "UIToFP",
  /* 42 */  "SIToFP",
  /* 43 */  "FPTrunc",
  /* 44 */  "FPExt",
  /* 45 */  "PtrToInt",
  /* 46 */  "IntToPtr",
  /* 47 */  "BitCast",
  /* 48 */  "AddrSpaceCast",
  /* 49 */  "CleanupPad",
  /* 50 */  "CatchPad",
  /* 51 */  "ICmp",
  /* 52 */  "FCmp",
  /* 53 */  "PHI",
  /* 54 */  "Call",
  /* 55 */  "Select",
  /* 56 */  "UserOp1",
  /* 57 */  "UserOp2",
  /* 58 */  "VAArg",
  /* 59 */  "ExtractElement",
  /* 60 */  "InsertElement",
  /* 61 */  "ShuffleVector",
  /* 62 */  "ExtractValue",
  /* 63 */  "InsertValue",
  /* 64 */  "LandingPad",
  /* 65 */  "REG",
  /* 66 */  "IMM",
  /* 67 */  "MEM",
  /* 68 */  "LABEL",
  /* 69 */  "ARGS",
  /* 70 */  "NOARGS",
  /* 71 */  "PTR",
};


std::string burm_string[] = {
  /* 0 */  "stmt: reg",
  /* 1 */  "stmt: mem",
  /* 2 */  "value: reg",
  /* 3 */  "value: imm",
  /* 4 */  "value: mem",
  /* 5 */  "ri: reg",
  /* 6 */  "ri: imm",
  /* 7 */  "rm: reg",
  /* 8 */  "rm: mem",
  /* 9 */  "reg: REG",
  /* 10 */  "imm: Load(IMM)",
  /* 11 */  "imm: IMM",
  /* 12 */  "mem: MEM",
  /* 13 */  "mem: ptr",
  /* 14 */  "label: LABEL",
  /* 15 */  "stmt: Ret(imm)",
  /* 16 */  "stmt: Ret(reg)",
  /* 17 */  "mem: Alloca(imm)",
  /* 18 */  "reg: Load(reg)",
  /* 19 */  "reg: Load(imm)",
  /* 20 */  "reg: Load(mem)",
  /* 21 */  "stmt: Store(imm,mem)",
  /* 22 */  "stmt: Store(reg,mem)",
  /* 23 */  "stmt: Store(mem,mem)",
  /* 24 */  "reg: Add(reg,ri)",
  /* 25 */  "reg: Add(ri,reg)",
  /* 26 */  "reg: Sub(reg,ri)",
  /* 27 */  "reg: Mul(reg,ri)",
  /* 28 */  "reg: Mul(ri,reg)",
  /* 29 */  "reg: UDiv(rm,rm)",
  /* 30 */  "reg: SDiv(rm,rm)",
  /* 31 */  "reg: UDiv(rm,imm)",
  /* 32 */  "reg: SDiv(rm,imm)",
  /* 33 */  "stmt: cond",
  /* 34 */  "cond: ICmp(imm,imm)",
  /* 35 */  "cond: ICmp(reg,imm)",
  /* 36 */  "cond: ICmp(reg,reg)",
  /* 37 */  "stmt: Br(label,DUMMY,DUMMY)",
  /* 38 */  "stmt: Br(cond,label,label)",
  /* 39 */  "reg: Call(args)",
  /* 40 */  "args: NOARGS",
  /* 41 */  "args: ARGS(value,args)",
  /* 42 */  "reg: Trunc(reg)",
  /* 43 */  "reg: BitCast(reg)",
  /* 44 */  "reg: BitCast(mem)",
  /* 45 */  "ptr: GetElementPtr(ri,ri,imm)",
  /* 46 */  "ptr: GetElementPtr(mem,ri,imm)",
};


std::string burm_files[] = {
"llc_olive.brg",
};

int burm_file_numbers[] = {
  /* 0 */  0,
  /* 1 */  0,
  /* 2 */  0,
  /* 3 */  0,
  /* 4 */  0,
  /* 5 */  0,
  /* 6 */  0,
  /* 7 */  0,
  /* 8 */  0,
  /* 9 */  0,
  /* 10 */  0,
  /* 11 */  0,
  /* 12 */  0,
  /* 13 */  0,
  /* 14 */  0,
  /* 15 */  0,
  /* 16 */  0,
  /* 17 */  0,
  /* 18 */  0,
  /* 19 */  0,
  /* 20 */  0,
  /* 21 */  0,
  /* 22 */  0,
  /* 23 */  0,
  /* 24 */  0,
  /* 25 */  0,
  /* 26 */  0,
  /* 27 */  0,
  /* 28 */  0,
  /* 29 */  0,
  /* 30 */  0,
  /* 31 */  0,
  /* 32 */  0,
  /* 33 */  0,
  /* 34 */  0,
  /* 35 */  0,
  /* 36 */  0,
  /* 37 */  0,
  /* 38 */  0,
  /* 39 */  0,
  /* 40 */  0,
  /* 41 */  0,
  /* 42 */  0,
  /* 43 */  0,
  /* 44 */  0,
  /* 45 */  0,
  /* 46 */  0,
};

int burm_line_numbers[] = {
  /* 0 */  28,
  /* 1 */  29,
  /* 2 */  31,
  /* 3 */  32,
  /* 4 */  33,
  /* 5 */  35,
  /* 6 */  36,
  /* 7 */  38,
  /* 8 */  39,
  /* 9 */  41,
  /* 10 */  42,
  /* 11 */  43,
  /* 12 */  44,
  /* 13 */  45,
  /* 14 */  46,
  /* 15 */  48,
  /* 16 */  64,
  /* 17 */  80,
  /* 18 */  116,
  /* 19 */  129,
  /* 20 */  138,
  /* 21 */  147,
  /* 22 */  156,
  /* 23 */  165,
  /* 24 */  179,
  /* 25 */  194,
  /* 26 */  209,
  /* 27 */  224,
  /* 28 */  239,
  /* 29 */  255,
  /* 30 */  272,
  /* 31 */  289,
  /* 32 */  313,
  /* 33 */  337,
  /* 34 */  338,
  /* 35 */  349,
  /* 36 */  359,
  /* 37 */  369,
  /* 38 */  373,
  /* 39 */  434,
  /* 40 */  445,
  /* 41 */  448,
  /* 42 */  462,
  /* 43 */  468,
  /* 44 */  478,
  /* 45 */  488,
  /* 46 */  517,
};

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"

static short burm_decode_stmt[] = {
   -1,
  0,
  1,
  15,
  16,
  21,
  22,
  23,
  33,
  37,
  38,
};

static short burm_decode_value[] = {
   -1,
  2,
  3,
  4,
};

static short burm_decode_ri[] = {
   -1,
  5,
  6,
};

static short burm_decode_rm[] = {
   -1,
  7,
  8,
};

static short burm_decode_reg[] = {
   -1,
  9,
  18,
  19,
  20,
  24,
  25,
  26,
  27,
  28,
  29,
  30,
  31,
  32,
  39,
  42,
  43,
  44,
};

static short burm_decode_imm[] = {
   -1,
  10,
  11,
};

static short burm_decode_mem[] = {
   -1,
  12,
  13,
  17,
};

static short burm_decode_ptr[] = {
   -1,
  45,
  46,
};

static short burm_decode_cond[] = {
   -1,
  34,
  35,
  36,
};

static short burm_decode_label[] = {
   -1,
  14,
};

static short burm_decode_args[] = {
   -1,
  40,
  41,
};

int burm_rule(struct burm_state *state, int goalnt) {
  burm_assert(goalnt >= 1 && goalnt <= 11,
        PANIC("Bad goal nonterminal %d in burm_rule\n", goalnt));
  if (!state)
    return 0;
  switch (goalnt) {
  case burm_stmt_NT:  return burm_decode_stmt[((struct burm_state *)state)->rule.burm_stmt];
  case burm_value_NT:  return burm_decode_value[((struct burm_state *)state)->rule.burm_value];
  case burm_ri_NT:  return burm_decode_ri[((struct burm_state *)state)->rule.burm_ri];
  case burm_rm_NT:  return burm_decode_rm[((struct burm_state *)state)->rule.burm_rm];
  case burm_reg_NT:  return burm_decode_reg[((struct burm_state *)state)->rule.burm_reg];
  case burm_imm_NT:  return burm_decode_imm[((struct burm_state *)state)->rule.burm_imm];
  case burm_mem_NT:  return burm_decode_mem[((struct burm_state *)state)->rule.burm_mem];
  case burm_ptr_NT:  return burm_decode_ptr[((struct burm_state *)state)->rule.burm_ptr];
  case burm_cond_NT:  return burm_decode_cond[((struct burm_state *)state)->rule.burm_cond];
  case burm_label_NT:  return burm_decode_label[((struct burm_state *)state)->rule.burm_label];
  case burm_args_NT:  return burm_decode_args[((struct burm_state *)state)->rule.burm_args];
  default:
    burm_assert(0, PANIC("Bad goal nonterminal %d in burm_rule\n", goalnt));
  }
  return 0;
}


struct burm_action {
  int nt;
  struct burm_state* st;
};

#ifndef RULE
#define RULE(n,s) \
    (act = (burm_action*) malloc(sizeof(struct burm_action)),act->nt=n,act->st=s,act)
#endif

int burm_cost_code(COST *_c, int _ern,struct burm_state *_s)
{
  NODEPTR *_children;
  struct burm_action *act;
  switch(_ern){
  default:
    DEFAULT_COST;
  case 0:
{


 return 1;                                         
}
  break;
  case 1:
{


 return 1;                                         
}
  break;
  case 2:
{


 (*_c).cost = _s->cost[burm_reg_NT].cost;                    
}
  break;
  case 3:
{


 (*_c).cost = _s->cost[burm_imm_NT].cost;                    
}
  break;
  case 4:
{


 (*_c).cost = _s->cost[burm_mem_NT].cost;                    
}
  break;
  case 5:
{


 (*_c).cost = _s->cost[burm_reg_NT].cost;                    
}
  break;
  case 6:
{


 (*_c).cost = _s->cost[burm_imm_NT].cost;                    
}
  break;
  case 7:
{


 (*_c).cost = _s->cost[burm_reg_NT].cost;                    
}
  break;
  case 8:
{


 (*_c).cost = _s->cost[burm_mem_NT].cost;                    
}
  break;
  case 9:
{


 (*_c).cost = 1;                                
}
  break;
  case 10:
{


 (*_c).cost = 0;                                
}
  break;
  case 11:
{


 (*_c).cost = 0;                                
}
  break;
  case 12:
{


 (*_c).cost = 10;                               
}
  break;
  case 13:
{


 return 1;                                         
}
  break;
  case 14:
{


 (*_c).cost = 1;                                
}
  break;
  case 15:
{


 (*_c).cost = _s->kids[0]->cost[burm_imm_NT].cost + 1;                
}
  break;
  case 16:
{


 (*_c).cost = _s->kids[0]->cost[burm_reg_NT].cost + 1;                
}
  break;
  case 17:
{


 (*_c).cost = _s->kids[0]->cost[burm_imm_NT].cost + 1;    
}
  break;
  case 18:
{


 (*_c).cost = _s->kids[0]->cost[burm_reg_NT].cost + 1;    
}
  break;
  case 19:
{


 (*_c).cost = _s->kids[0]->cost[burm_imm_NT].cost + 1;    
}
  break;
  case 20:
{


 (*_c).cost = _s->kids[0]->cost[burm_mem_NT].cost + 1;    
}
  break;
  case 21:
{


 (*_c).cost = _s->kids[0]->cost[burm_imm_NT].cost + _s->kids[1]->cost[burm_mem_NT].cost;    
}
  break;
  case 22:
{


 (*_c).cost = _s->kids[0]->cost[burm_reg_NT].cost + _s->kids[1]->cost[burm_mem_NT].cost;    
}
  break;
  case 23:
{


 (*_c).cost = _s->kids[0]->cost[burm_mem_NT].cost + _s->kids[1]->cost[burm_mem_NT].cost;    
}
  break;
  case 24:
{


 (*_c).cost = _s->kids[0]->cost[burm_reg_NT].cost + _s->kids[1]->cost[burm_ri_NT].cost + 2;    
}
  break;
  case 25:
{


 (*_c).cost = _s->kids[0]->cost[burm_ri_NT].cost + _s->kids[1]->cost[burm_reg_NT].cost + 2;    
}
  break;
  case 26:
{


 (*_c).cost = _s->kids[0]->cost[burm_reg_NT].cost + _s->kids[1]->cost[burm_ri_NT].cost + 1;    
}
  break;
  case 27:
{


 (*_c).cost = _s->kids[0]->cost[burm_reg_NT].cost + _s->kids[1]->cost[burm_ri_NT].cost + 2;    
}
  break;
  case 28:
{


 (*_c).cost = _s->kids[0]->cost[burm_ri_NT].cost + _s->kids[1]->cost[burm_reg_NT].cost + 2;    
}
  break;
  case 29:
{


 (*_c).cost = _s->kids[0]->cost[burm_rm_NT].cost + _s->kids[1]->cost[burm_rm_NT].cost + 4;    
}
  break;
  case 30:
{


 (*_c).cost = _s->kids[0]->cost[burm_rm_NT].cost + _s->kids[1]->cost[burm_rm_NT].cost + 4;    
}
  break;
  case 31:
{


 (*_c).cost = _s->kids[0]->cost[burm_rm_NT].cost + _s->kids[1]->cost[burm_imm_NT].cost + 4;    
}
  break;
  case 32:
{


 (*_c).cost = _s->kids[0]->cost[burm_rm_NT].cost + _s->kids[1]->cost[burm_imm_NT].cost + 4;    
}
  break;
  case 33:
{


 (*_c).cost = _s->cost[burm_cond_NT].cost;                       
}
  break;
  case 34:
{


 (*_c).cost = _s->kids[0]->cost[burm_imm_NT].cost + _s->kids[1]->cost[burm_imm_NT].cost;       
}
  break;
  case 35:
{


 (*_c).cost = _s->kids[0]->cost[burm_reg_NT].cost + _s->kids[1]->cost[burm_imm_NT].cost;       
}
  break;
  case 36:
{


 (*_c).cost = _s->kids[0]->cost[burm_reg_NT].cost + _s->kids[1]->cost[burm_reg_NT].cost;       
}
  break;
  case 37:
{


 (*_c).cost = _s->kids[0]->cost[burm_label_NT].cost + 5; 
}
  break;
  case 38:
{


 (*_c).cost = _s->kids[0]->cost[burm_cond_NT].cost + _s->kids[1]->cost[burm_label_NT].cost + _s->kids[2]->cost[burm_label_NT].cost + 5; 
}
  break;
  case 39:
{


 (*_c).cost = _s->kids[0]->cost[burm_args_NT].cost + 5; 
}
  break;
  case 40:
{


 (*_c).cost = 0;                  
}
  break;
  case 41:
{


 (*_c).cost = _s->kids[0]->cost[burm_value_NT].cost + _s->kids[1]->cost[burm_args_NT].cost; 
}
  break;
  case 42:
{


 (*_c).cost = _s->kids[0]->cost[burm_reg_NT].cost + 3;              
}
  break;
  case 43:
{


 (*_c).cost = _s->kids[0]->cost[burm_reg_NT].cost + 1; 
}
  break;
  case 44:
{


 (*_c).cost = _s->kids[0]->cost[burm_mem_NT].cost + 1; 
}
  break;
  case 45:
{


 (*_c).cost = _s->kids[0]->cost[burm_ri_NT].cost + _s->kids[1]->cost[burm_ri_NT].cost + _s->kids[2]->cost[burm_imm_NT].cost + 5; 
}
  break;
  case 46:
{


 (*_c).cost = _s->kids[0]->cost[burm_mem_NT].cost + _s->kids[1]->cost[burm_ri_NT].cost + _s->kids[2]->cost[burm_imm_NT].cost + 5; 
}
  break;
  }
  return 1;
}


void stmt_action(struct burm_state *_s, 

FUNCTION_STATE fstate);


void value_action(struct burm_state *_s, 

FUNCTION_STATE fstate);


void ri_action(struct burm_state *_s, 

FUNCTION_STATE fstate);


void rm_action(struct burm_state *_s, 

FUNCTION_STATE fstate);


void reg_action(struct burm_state *_s, 

FUNCTION_STATE fstate);


void imm_action(struct burm_state *_s, 

FUNCTION_STATE fstate);


void mem_action(struct burm_state *_s, 

FUNCTION_STATE fstate);


void ptr_action(struct burm_state *_s, 

FUNCTION_STATE fstate);


void cond_action(struct burm_state *_s, 

FUNCTION_STATE fstate);


void label_action(struct burm_state *_s, 

FUNCTION_STATE fstate);


void args_action(struct burm_state *_s, 

FUNCTION_STATE fstate, int n);


#include <stdarg.h>

void burm_exec(struct burm_state *state, int nterm, ...) 
{
  va_list(ap);
  va_start(ap,nterm);

  burm_assert(nterm >= 1 && nterm <= 11,
        PANIC("Bad nonterminal %d in $exec\n", nterm));

  if (state)
    switch (nterm) {
    case burm_stmt_NT:
      stmt_action(state,va_arg(ap,FUNCTION_STATE));
      break;
    case burm_value_NT:
      value_action(state,va_arg(ap,FUNCTION_STATE));
      break;
    case burm_ri_NT:
      ri_action(state,va_arg(ap,FUNCTION_STATE));
      break;
    case burm_rm_NT:
      rm_action(state,va_arg(ap,FUNCTION_STATE));
      break;
    case burm_reg_NT:
      reg_action(state,va_arg(ap,FUNCTION_STATE));
      break;
    case burm_imm_NT:
      imm_action(state,va_arg(ap,FUNCTION_STATE));
      break;
    case burm_mem_NT:
      mem_action(state,va_arg(ap,FUNCTION_STATE));
      break;
    case burm_ptr_NT:
      ptr_action(state,va_arg(ap,FUNCTION_STATE));
      break;
    case burm_cond_NT:
      cond_action(state,va_arg(ap,FUNCTION_STATE));
      break;
    case burm_label_NT:
      label_action(state,va_arg(ap,FUNCTION_STATE));
      break;
    case burm_args_NT:
      args_action(state,va_arg(ap,FUNCTION_STATE),va_arg(ap,int));
      break;
    default:
      PANIC("Bad nonterminal %d in $exec\n", nterm);
      break;
    }
  else
    PANIC("Bad state for $exec in nonterminal %d \n",nterm);
  va_end(ap);
}

#define EXEC(s,n,a) ( \
  (n == burm_stmt_NT)? burm_exec(s,n,a): \
  (n == burm_value_NT)? burm_exec(s,n,a): \
  (n == burm_ri_NT)? burm_exec(s,n,a): \
  (n == burm_rm_NT)? burm_exec(s,n,a): \
  (n == burm_reg_NT)? burm_exec(s,n,a): \
  (n == burm_imm_NT)? burm_exec(s,n,a): \
  (n == burm_mem_NT)? burm_exec(s,n,a): \
  (n == burm_ptr_NT)? burm_exec(s,n,a): \
  (n == burm_cond_NT)? burm_exec(s,n,a): \
  (n == burm_label_NT)? burm_exec(s,n,a): \
  (n == burm_args_NT)? burm_exec(s,n,a): \
  PANIC("Bad nonterminal %d in $exec\n", n))

struct burm_state *burm_immed(struct burm_state *s,int n);
#ifndef NO_ACTION
#define NO_ACTION assert(0)
#endif


void stmt_action(struct burm_state *_s, 

FUNCTION_STATE fstate)
{
  struct burm_state *_t;
  int _ern=burm_decode_stmt[_s->rule.burm_stmt];
  NODEPTR *_children;
  if(_s->rule.burm_stmt==0)
    NO_ACTION(stmt);
  switch(_ern){
  case 0:
{


 reg_action(_s,fstate); 
}
  break;
  case 1:
{


 mem_action(_s,fstate); 
}
  break;
  case 15:
{



            /*
             * IR Ret takes a register argument
             * X86 Ret takes no argument, return value is passed by %eax
             */
            imm_action(_s->kids[0],fstate);
            fstate->CreatePhysicalReg(_s->node, RAX);
            fstate->LoadFromImm(_s->node, _s->kids[0]->node);
            // this will record the ending in liveness range for a register
            // if the input is a register, and the register dies
            fstate->RecordLiveness(_s->kids[0]->node);
            std::stringstream ss;
            ss << "jmp \t.LFE" << fstate->GetFunctionID();
            std::string s = ss.str();
            fstate->GenerateStmt(s.c_str());
        
}
  break;
  case 16:
{



            /*
             * IR Ret takes a register argument
             * X86 Ret takes no argument, return value is passed by %eax
             */
            reg_action(_s->kids[0],fstate);
            fstate->CreatePhysicalReg(_s->node, RAX);
            fstate->LoadFromReg(_s->node, _s->kids[0]->node);
            // this will record the ending in liveness range for a register
            // if the input is a register, and the register dies
            fstate->RecordLiveness(_s->kids[0]->node);
            std::stringstream ss;
            ss << "jmp \t.LFE" << fstate->GetFunctionID();
            std::string s = ss.str();
            fstate->GenerateStmt(s.c_str());
        
}
  break;
  case 21:
{



            imm_action(_s->kids[0],fstate);
            mem_action(_s->kids[1],fstate);
            fstate->GenerateMovStmt(_s->kids[1]->node, _s->kids[0]->node, _s->kids[1]->node->GetSuffix());
            // this will record the ending in liveness range for a register
            // if the input is a register, and the register dies
            fstate->RecordLiveness(_s->kids[0]->node);
            fstate->RecordLiveness(_s->kids[1]->node);
        
}
  break;
  case 22:
{



            reg_action(_s->kids[0],fstate);
            mem_action(_s->kids[1],fstate);
            fstate->GenerateMovStmt(_s->kids[1]->node, _s->kids[0]->node, _s->kids[1]->node->GetSuffix());
            // this will record the ending in liveness range for a register
            // if the input is a register, and the register dies
            fstate->RecordLiveness(_s->kids[0]->node);
            fstate->RecordLiveness(_s->kids[1]->node);
        
}
  break;
  case 23:
{



            mem_action(_s->kids[0],fstate);
            mem_action(_s->kids[1],fstate);

            Tree *t1 = (new Tree(REG))->GetTreeRef();
            fstate->CreateVirtualReg(t1);

            fstate->GenerateMovStmt(t1, _s->kids[0]->node, _s->kids[1]->node->GetSuffix());
            fstate->GenerateMovStmt(_s->kids[1]->node, t1, _s->kids[1]->node->GetSuffix());

            fstate->RecordLiveness(t1);
            fstate->RecordLiveness(_s->kids[0]->node);
            fstate->RecordLiveness(_s->kids[1]->node);
        
}
  break;
  case 33:
{


 cond_action(_s,fstate); 
}
  break;
  case 37:
{



            fstate->GenerateUnaryStmt("jmp ", _s->kids[0]->node, false);
            label_action(_s->kids[0],fstate);
        
}
  break;
  case 38:
{



            using namespace llvm;

            cond_action(_s->kids[0],fstate);
            CmpInst *inst = dyn_cast<CmpInst>(_s->kids[0]->node->GetInst());
            assert(inst && "cond node must have instruction attached");

            /**
             * As our comparison is reversed, we need to reverse the comparison predicate
             * */
            std::string op;
            CmpInst::Predicate pred = inst->getPredicate();
            switch (pred) {
                case CmpInst::Predicate::ICMP_EQ:
                    // equal
                    op = "jne ";
                    break;
                case CmpInst::Predicate::ICMP_NE:
                    // not equal
                    op = "je  ";
                    break;
                case CmpInst::Predicate::ICMP_UGT:
                    // unsigned greater than
                    op = "jbe ";
                    break;
                case CmpInst::Predicate::ICMP_UGE:
                    // unsigned greater or equal
                    op = "jb  ";
                    break;
                case CmpInst::Predicate::ICMP_ULT:
                    // unsigned less than
                    op = "jae ";
                    break;
                case CmpInst::Predicate::ICMP_ULE:
                    // unsigned less or equal
                    op = "ja  ";
                    break;
                case CmpInst::Predicate::ICMP_SGT:
                    // signed greater than
                    op = "jle ";
                    break;
                case CmpInst::Predicate::ICMP_SGE:
                    // signed greater or equal
                    op = "jl  ";
                    break;
                case CmpInst::Predicate::ICMP_SLT:
                    // signed less than
                    op = "jge ";
                    break;
                case CmpInst::Predicate::ICMP_SLE:
                    // signed less or equal
                    op = "jg  ";
                    break;
                default:
                    assert(false && "FP TYPES NOT SUPPORTED!");
            }
            fstate->GenerateUnaryStmt(op.c_str(), _s->kids[1]->node, false);

            label_action(_s->kids[2],fstate);
            label_action(_s->kids[1],fstate);
        
}
  break;
  }
}


void value_action(struct burm_state *_s, 

FUNCTION_STATE fstate)
{
  struct burm_state *_t;
  int _ern=burm_decode_value[_s->rule.burm_value];
  NODEPTR *_children;
  if(_s->rule.burm_value==0)
    NO_ACTION(value);
  switch(_ern){
  case 2:
{


 reg_action(_s,fstate); 
}
  break;
  case 3:
{


 imm_action(_s,fstate); 
}
  break;
  case 4:
{


 mem_action(_s,fstate); 
}
  break;
  }
}


void ri_action(struct burm_state *_s, 

FUNCTION_STATE fstate)
{
  struct burm_state *_t;
  int _ern=burm_decode_ri[_s->rule.burm_ri];
  NODEPTR *_children;
  if(_s->rule.burm_ri==0)
    NO_ACTION(ri);
  switch(_ern){
  case 5:
{


 reg_action(_s,fstate); 
}
  break;
  case 6:
{


 imm_action(_s,fstate); 
}
  break;
  }
}


void rm_action(struct burm_state *_s, 

FUNCTION_STATE fstate)
{
  struct burm_state *_t;
  int _ern=burm_decode_rm[_s->rule.burm_rm];
  NODEPTR *_children;
  if(_s->rule.burm_rm==0)
    NO_ACTION(rm);
  switch(_ern){
  case 7:
{


 reg_action(_s,fstate); 
}
  break;
  case 8:
{


 mem_action(_s,fstate); 
}
  break;
  }
}


void reg_action(struct burm_state *_s, 

FUNCTION_STATE fstate)
{
  struct burm_state *_t;
  int _ern=burm_decode_reg[_s->rule.burm_reg];
  NODEPTR *_children;
  if(_s->rule.burm_reg==0)
    NO_ACTION(reg);
  switch(_ern){
  case 9:
{


 _s->node->UseAsRegister(); 
}
  break;
  case 18:
{



            /*
             * IR Load takes one arguments for allocation size
             * X86 uses mov instruction, which takes two arguments
             */
            reg_action(_s->kids[0],fstate);
            fstate->AssignVirtualReg(_s->node, _s->kids[0]->node);    // assign a virtual register to the inst
            fstate->LoadFromReg(_s->node, _s->kids[0]->node);
            // this will record the ending in liveness range for a register
            // if the input is a register, and the register dies
            fstate->RecordLiveness(_s->kids[0]->node);
            _s->node->UseAsRegister();
        
}
  break;
  case 19:
{



            imm_action(_s->kids[0],fstate);
            fstate->CreateVirtualReg(_s->node);      // assign a virtual register to the inst
            fstate->GenerateMovStmt(_s->node, _s->kids[0]->node);
            // this will record the ending in liveness range for a register
            // if the input is a register, and the register dies
            fstate->RecordLiveness(_s->kids[0]->node);
            _s->node->UseAsRegister();
        
}
  break;
  case 20:
{



            mem_action(_s->kids[0],fstate);
            fstate->CreateVirtualReg(_s->node);      // assign a virtual register to the inst
            fstate->GenerateMovStmt(_s->node, _s->kids[0]->node);
            // this will record the ending in liveness range for a register
            // if the input is a register, and the register dies
            fstate->RecordLiveness(_s->kids[0]->node);
            _s->node->UseAsRegister();
        
}
  break;
  case 24:
{



            /*
             * IR Add takes two arguments
             * X86 Add takes two argument, both operands cannot be from memory in the instruction
             */
            reg_action(_s->kids[0],fstate);
            ri_action(_s->kids[1],fstate);
            fstate->AssignVirtualReg(_s->node, _s->kids[0]->node);    // assign a virtual register to the inst
            fstate->GenerateBinaryStmt("add", _s->node, _s->kids[1]->node);
            // this will record the ending in liveness range for a register
            // if the input is a register, and the register dies
            fstate->RecordLiveness(_s->kids[0]->node);
            fstate->RecordLiveness(_s->kids[1]->node);
            _s->node->UseAsRegister();
        
}
  break;
  case 25:
{



            /*
             * IR Add takes two arguments
             * X86 Add takes two argument, both operands cannot be from memory in the instruction
             */
            ri_action(_s->kids[0],fstate);
            reg_action(_s->kids[1],fstate);
            fstate->AssignVirtualReg(_s->node, _s->kids[1]->node);    // assign a virtual register to the inst
            fstate->GenerateBinaryStmt("add", _s->node, _s->kids[0]->node);
            // this will record the ending in liveness range for a register
            // if the input is a register, and the register dies
            fstate->RecordLiveness(_s->kids[0]->node);
            fstate->RecordLiveness(_s->kids[1]->node);
            _s->node->UseAsRegister();
        
}
  break;
  case 26:
{



            /*
             * IR Sub takes two arguments
             * X86 Sub takes two argument, both operands cannot be from memory in the instruction
             */
            reg_action(_s->kids[0],fstate);
            ri_action(_s->kids[1],fstate);
            fstate->AssignVirtualReg(_s->node, _s->kids[0]->node);    // assign a virtual register to the inst
            fstate->GenerateBinaryStmt("sub", _s->node, _s->kids[1]->node);
            // this will record the ending in liveness range for a register
            // if the input is a register, and the register dies
            fstate->RecordLiveness(_s->kids[0]->node);
            fstate->RecordLiveness(_s->kids[1]->node);
            _s->node->UseAsRegister();
        
}
  break;
  case 27:
{



            /*
             * IR Mul takes two arguments
             * X86 Mul takes two argument, both operands cannot be from memory in the instruction
             */
            reg_action(_s->kids[0],fstate);
            ri_action(_s->kids[1],fstate);
            fstate->AssignVirtualReg(_s->node, _s->kids[0]->node);    // assign a virtual register to the inst
            fstate->GenerateBinaryStmt("imul", _s->node, _s->kids[1]->node);
            // this will record the ending in liveness range for a register
            // if the input is a register, and the register dies
            fstate->RecordLiveness(_s->kids[0]->node);
            fstate->RecordLiveness(_s->kids[1]->node);
            _s->node->UseAsRegister();
        
}
  break;
  case 28:
{



            /*
             * IR Mul takes two arguments
             * X86 Mul takes two argument, both operands cannot be from memory in the instruction
             */
            ri_action(_s->kids[0],fstate);
            reg_action(_s->kids[1],fstate);
            // std::cerr << "IS REG A PHYSICAL REG? " << _s->kids[1]->node->IsPhysicalReg() << std::endl;
            fstate->AssignVirtualReg(_s->node, _s->kids[1]->node);    // assign a virtual register to the inst
            fstate->GenerateBinaryStmt("imul", _s->node, _s->kids[0]->node);
            // this will record the ending in liveness range for a register
            // if the input is a register, and the register dies
            fstate->RecordLiveness(_s->kids[0]->node);
            fstate->RecordLiveness(_s->kids[1]->node);
            _s->node->UseAsRegister();
        
}
  break;
  case 29:
{



            /*
             * IR Mul takes two arguments
             * X86 Mul takes one argument, perform %rax = %rax / src
             */
            rm_action(_s->kids[0],fstate);
            rm_action(_s->kids[1],fstate);

            fstate->CreatePhysicalReg(_s->node, RAX);         // the result is stored in %rax
            fstate->GenerateMovStmt(_s->node, _s->kids[0]->node);            // move one of the operands to %rax
            fstate->GenerateStmt("cqto");               // sign extension
            fstate->GenerateUnaryStmt("div", _s->kids[1]->node);       // generate div instruction

            fstate->RecordLiveness(_s->kids[0]->node);
            fstate->RecordLiveness(_s->kids[1]->node);
            _s->node->UseAsRegister();
        
}
  break;
  case 30:
{



            /*
             * IR Mul takes two arguments
             * X86 Mul takes one argument, perform %rax = %rax / src
             */
            rm_action(_s->kids[0],fstate);
            rm_action(_s->kids[1],fstate);

            fstate->CreatePhysicalReg(_s->node, RAX);         // the result is stored in %rax
            fstate->GenerateMovStmt(_s->node, _s->kids[0]->node);            // move one of the operands to %rax
            fstate->GenerateStmt("cqto");               // sign extension
            fstate->GenerateUnaryStmt("idiv", _s->kids[1]->node);      // generate div instruction

            fstate->RecordLiveness(_s->kids[0]->node);
            fstate->RecordLiveness(_s->kids[1]->node);
            _s->node->UseAsRegister();
        
}
  break;
  case 31:
{



            /*
             * IR Mul takes two arguments
             * X86 Mul takes one argument, perform %rax = %rax / src
             */
            rm_action(_s->kids[0],fstate);
            imm_action(_s->kids[1],fstate);

            fstate->CreatePhysicalReg(_s->node, RAX);         // the result is stored in %rax
            fstate->GenerateMovStmt(_s->node, _s->kids[0]->node);            // move one of the operands to %rax

            Tree *t1 = (new Tree(REG))->GetTreeRef();
            fstate->CreateVirtualReg(t1);
            fstate->GenerateMovStmt(t1, _s->kids[1]->node);            // move one of the operands to new register

            fstate->GenerateStmt("cqto");               // sign extension
            fstate->GenerateUnaryStmt("div", t1);       // generate div instruction

            fstate->RecordLiveness(t1);
            fstate->RecordLiveness(_s->kids[0]->node);
            fstate->RecordLiveness(_s->kids[0]->node);
            fstate->RecordLiveness(_s->kids[1]->node);
            _s->node->UseAsRegister();
        
}
  break;
  case 32:
{



            /*
             * IR Mul takes two arguments
             * X86 Mul takes one argument, perform %rax = %rax / src
             */
            rm_action(_s->kids[0],fstate);
            imm_action(_s->kids[1],fstate);

            fstate->CreatePhysicalReg(_s->node, RAX);         // the result is stored in %rax
            fstate->GenerateMovStmt(_s->node, _s->kids[0]->node);            // move one of the operands to %rax

            Tree *t1 = (new Tree(REG))->GetTreeRef();
            fstate->CreateVirtualReg(t1);
            fstate->GenerateMovStmt(t1, _s->kids[1]->node);            // move one of the operands to new register

            fstate->GenerateStmt("cqto");               // sign extension
            fstate->GenerateUnaryStmt("idiv", t1);       // generate div instruction

            fstate->RecordLiveness(t1);
            fstate->RecordLiveness(_s->kids[0]->node);
            fstate->RecordLiveness(_s->kids[1]->node);
            _s->node->UseAsRegister();
        
}
  break;
  case 39:
{



            args_action(_s->kids[0],fstate, 0);

            std::stringstream ss;
            ss << "call\t" << _s->node->GetFuncName();
            std::string s = ss.str();
            fstate->GenerateStmt(s.c_str());

            fstate->CreatePhysicalReg(_s->node, RAX);
            _s->node->UseAsPhysicalRegister();
        
}
  break;
  case 42:
{



            reg_action(_s->kids[0],fstate);
            fstate->CreateVirtualReg(_s->node);
            fstate->LoadFromReg(_s->node, _s->kids[0]->node);
            fstate->RecordLiveness(_s->kids[0]->node);
        
}
  break;
  case 43:
{



            if (_s->node->IsComputed()) return;
            _s->node->SetComputed(true);

            // std::cerr << "=============BitCast REG REF " << _s->node->GetRefCount() << std::endl;
            reg_action(_s->kids[0],fstate);
            fstate->AssignVirtualReg(_s->node, _s->kids[0]->node);    // assign a virtual register to the inst
            fstate->RecordLiveness(_s->kids[0]->node);
            _s->node->UseAsRegister();
        
}
  break;
  case 44:
{



            if (_s->node->IsComputed()) return;
            _s->node->SetComputed(true);

            // std::cerr << "-------------BitCast REG REF " << _s->node->GetRefCount() << std::endl;
            mem_action(_s->kids[0],fstate);
            fstate->CreateVirtualReg(_s->node);    // assign a virtual register to the inst
            fstate->LoadEffectiveAddress(_s->node, _s->kids[0]->node);
            _s->node->UseAsRegister();
        
}
  break;
  }
}


void imm_action(struct burm_state *_s, 

FUNCTION_STATE fstate)
{
  struct burm_state *_t;
  int _ern=burm_decode_imm[_s->rule.burm_imm];
  NODEPTR *_children;
  if(_s->rule.burm_imm==0)
    NO_ACTION(imm);
  switch(_ern){
  case 10:
{


 _s->node->UseAsImmediate(); 
}
  break;
  case 11:
{


 _s->node->UseAsImmediate(); 
}
  break;
  }
}


void mem_action(struct burm_state *_s, 

FUNCTION_STATE fstate)
{
  struct burm_state *_t;
  int _ern=burm_decode_mem[_s->rule.burm_mem];
  NODEPTR *_children;
  if(_s->rule.burm_mem==0)
    NO_ACTION(mem);
  switch(_ern){
  case 12:
{


 _s->node->UseAsMemory(); 
}
  break;
  case 13:
{


 ptr_action(_s,fstate); _s->node->UseAsMemory(); 
}
  break;
  case 17:
{



            /*
             * IR Alloca takes one arguments for allocation size
             * X86 does not have correspondence for Alloca, but only uses $esp - size
             */
            using namespace llvm;
            llvm::AllocaInst *inst = dyn_cast<llvm::AllocaInst>(_s->node->GetInst());
            assert(inst && "llvm inst must be alloca inst");

            imm_action(_s->kids[0],fstate);

            Type *type = inst->getAllocatedType();
            int type_size = GetTypeSize(type);

            if (inst->isArrayAllocation()) {
                llvm::ConstantInt* array_size = dyn_cast<llvm::ConstantInt>(inst->getArraySize());
                int elems = array_size->getZExtValue();
                fstate->CreateLocal(_s->node, type_size * elems);     // reserve the space for array elements
            }
            else if (type->isArrayTy()) {
                ArrayType *arr_ty = dyn_cast<ArrayType>(type);
                int elems = arr_ty->getNumElements();
                fstate->CreateLocal(_s->node, type_size * elems);     // reserve the space for array elements
            }
            else if (type->isStructTy()) {
                assert(false && "struct not supported");
            }
            else {
                fstate->CreateLocal(_s->node, type_size);
            }

            // this will record the ending in liveness range for a register
            // if the input is a register, and the register dies
            fstate->RecordLiveness(_s->kids[0]->node);
            _s->node->UseAsMemory();
        
}
  break;
  }
}


void ptr_action(struct burm_state *_s, 

FUNCTION_STATE fstate)
{
  struct burm_state *_t;
  int _ern=burm_decode_ptr[_s->rule.burm_ptr];
  NODEPTR *_children;
  if(_s->rule.burm_ptr==0)
    NO_ACTION(ptr);
  switch(_ern){
  case 45:
{



            if (_s->node->IsComputed()) return;
            _s->node->SetComputed(true);

            // get size of array elements
            using namespace llvm;
            GetElementPtrInst *inst = dyn_cast<GetElementPtrInst>(_s->node->GetInst());
            assert(inst && "GetElementPtr must be not null");
            int size = GetTypeSize(inst->getSourceElementType());
            // std::cerr << "SIZE1: " << size << std::endl;

            ri_action(_s->kids[0],fstate);
            ri_action(_s->kids[1],fstate);
            imm_action(_s->kids[2],fstate);
            X86Operand *ptr = new X86Operand(fstate, OP_TYPE::X86Mem,
                _s->kids[0]->node->AsX86Operand(fstate),         // base
                _s->kids[1]->node->AsX86Operand(fstate),         // displacement
                0,
                size * _s->kids[2]->node->GetValue().val.i64s    // offset
            );
            _s->node->SetX86Operand(ptr);
            _s->node->SetSuffix(GetTypeSuffix(inst->getSourceElementType()));

            fstate->RecordLiveness(_s->kids[0]->node);
            fstate->RecordLiveness(_s->kids[1]->node);
            fstate->RecordLiveness(_s->kids[2]->node);
            _s->node->UseAsMemory();
            _s->node->UseAsPtr();
        
}
  break;
  case 46:
{



            if (_s->node->IsComputed()) return;
            _s->node->SetComputed(true);

            // get size of array elements
            using namespace llvm;
            GetElementPtrInst *inst = dyn_cast<GetElementPtrInst>(_s->node->GetInst());
            assert(inst && "GetElementPtr must be not null");
            int size = GetTypeSize(inst->getSourceElementType());
            // std::cerr << "SIZE2: " << size << std::endl;
            const char *suffix = GetTypeSuffix(inst->getSourceElementType()).c_str();

            mem_action(_s->kids[0],fstate);
            ri_action(_s->kids[1],fstate);
            imm_action(_s->kids[2],fstate);

            Tree *t1 = (new Tree(REG))->GetTreeRef();
            fstate->CreateVirtualReg(t1);
            fstate->LoadEffectiveAddress(t1, _s->kids[0]->node);

            X86Operand *ptr = new X86Operand(fstate, OP_TYPE::X86Mem,
                t1->AsX86Operand(fstate),         // base
                _s->kids[1]->node->AsX86Operand(fstate),         // displacement
                0,
                size * _s->kids[2]->node->GetValue().val.i64s       // offset
            );
            _s->node->SetX86Operand(ptr);
            _s->node->SetSuffix(suffix);

            fstate->RecordLiveness(t1);
            fstate->RecordLiveness(_s->kids[0]->node);
            fstate->RecordLiveness(_s->kids[1]->node);
            fstate->RecordLiveness(_s->kids[2]->node);
            _s->node->UseAsMemory();
            _s->node->UseAsPtr();
        
}
  break;
  }
}


void cond_action(struct burm_state *_s, 

FUNCTION_STATE fstate)
{
  struct burm_state *_t;
  int _ern=burm_decode_cond[_s->rule.burm_cond];
  NODEPTR *_children;
  if(_s->rule.burm_cond==0)
    NO_ACTION(cond);
  switch(_ern){
  case 34:
{


 
            imm_action(_s->kids[0],fstate);
            imm_action(_s->kids[1],fstate);
            fstate->CreateVirtualReg(_s->node);
            fstate->GenerateMovStmt(_s->node, _s->kids[0]->node);
            fstate->GenerateBinaryStmt("cmp", _s->node, _s->kids[1]->node);
            // this will record the ending in liveness range for a register
            // if the input is a register, and the register dies
            fstate->RecordLiveness(_s->kids[0]->node);
            fstate->RecordLiveness(_s->kids[1]->node);
        
}
  break;
  case 35:
{


 
            reg_action(_s->kids[0],fstate);
            imm_action(_s->kids[1],fstate);
            fstate->AssignVirtualReg(_s->node, _s->kids[0]->node);    // assign a virtual register to the inst
            fstate->GenerateBinaryStmt("cmp", _s->node, _s->kids[1]->node);
            // this will record the ending in liveness range for a register
            // if the input is a register, and the register dies
            fstate->RecordLiveness(_s->kids[0]->node);
            fstate->RecordLiveness(_s->kids[1]->node);
        
}
  break;
  case 36:
{


 
            reg_action(_s->kids[0],fstate);
            reg_action(_s->kids[1],fstate);
            fstate->AssignVirtualReg(_s->node, _s->kids[0]->node);    // assign a virtual register to the inst
            fstate->GenerateBinaryStmt("cmp", _s->node, _s->kids[1]->node);
            // this will record the ending in liveness range for a register
            // if the input is a register, and the register dies
            fstate->RecordLiveness(_s->kids[0]->node);
            fstate->RecordLiveness(_s->kids[1]->node);
        
}
  break;
  }
}


void label_action(struct burm_state *_s, 

FUNCTION_STATE fstate)
{
  struct burm_state *_t;
  int _ern=burm_decode_label[_s->rule.burm_label];
  NODEPTR *_children;
  if(_s->rule.burm_label==0)
    NO_ACTION(label);
  switch(_ern){
  case 14:
{


 
}
  break;
  }
}


void args_action(struct burm_state *_s, 

FUNCTION_STATE fstate, int n)
{
  struct burm_state *_t;
  int _ern=burm_decode_args[_s->rule.burm_args];
  NODEPTR *_children;
  if(_s->rule.burm_args==0)
    NO_ACTION(args);
  switch(_ern){
  case 40:
{



            // for args tree termination
        
}
  break;
  case 41:
{



            value_action(_s->kids[0],fstate);
            args_action(_s->kids[1],fstate, n + 1);
            if (n < 6) {
                _s->node->SetValue(params_regs[n]);
                _s->node->UseAsPhysicalRegister();
                if (!_s->kids[0]->node->IsPointer()) fstate->GenerateMovStmt(_s->node, _s->kids[0]->node);
                else                  fstate->LoadEffectiveAddress(_s->node, _s->kids[0]->node);
            } else {
                // more than 6 argments should be pushed on stack in reversed order
                fstate->GeneratePushStmt(_s->kids[0]->node);
            }
            fstate->RecordLiveness(_s->kids[0]->node);
        
}
  break;
  }
}
static void burm_closure_reg(struct burm_state *, COST);
static void burm_closure_imm(struct burm_state *, COST);
static void burm_closure_mem(struct burm_state *, COST);
static void burm_closure_ptr(struct burm_state *, COST);
static void burm_closure_cond(struct burm_state *, COST);

static void burm_closure_reg(struct burm_state *s, COST c) {
  if(burm_cost_code(&c,7,s) && COST_LESS(c,s->cost[burm_rm_NT])) {
burm_trace(burm_np, 7, c);     s->cost[burm_rm_NT] = c ;
    s->rule.burm_rm = 1;
  }
  if(burm_cost_code(&c,5,s) && COST_LESS(c,s->cost[burm_ri_NT])) {
burm_trace(burm_np, 5, c);     s->cost[burm_ri_NT] = c ;
    s->rule.burm_ri = 1;
  }
  if(burm_cost_code(&c,2,s) && COST_LESS(c,s->cost[burm_value_NT])) {
burm_trace(burm_np, 2, c);     s->cost[burm_value_NT] = c ;
    s->rule.burm_value = 1;
  }
  if(burm_cost_code(&c,0,s) && COST_LESS(c,s->cost[burm_stmt_NT])) {
burm_trace(burm_np, 0, c);     s->cost[burm_stmt_NT] = c ;
    s->rule.burm_stmt = 1;
  }
}

static void burm_closure_imm(struct burm_state *s, COST c) {
  if(burm_cost_code(&c,6,s) && COST_LESS(c,s->cost[burm_ri_NT])) {
burm_trace(burm_np, 6, c);     s->cost[burm_ri_NT] = c ;
    s->rule.burm_ri = 2;
  }
  if(burm_cost_code(&c,3,s) && COST_LESS(c,s->cost[burm_value_NT])) {
burm_trace(burm_np, 3, c);     s->cost[burm_value_NT] = c ;
    s->rule.burm_value = 2;
  }
}

static void burm_closure_mem(struct burm_state *s, COST c) {
  if(burm_cost_code(&c,8,s) && COST_LESS(c,s->cost[burm_rm_NT])) {
burm_trace(burm_np, 8, c);     s->cost[burm_rm_NT] = c ;
    s->rule.burm_rm = 2;
  }
  if(burm_cost_code(&c,4,s) && COST_LESS(c,s->cost[burm_value_NT])) {
burm_trace(burm_np, 4, c);     s->cost[burm_value_NT] = c ;
    s->rule.burm_value = 3;
  }
  if(burm_cost_code(&c,1,s) && COST_LESS(c,s->cost[burm_stmt_NT])) {
burm_trace(burm_np, 1, c);     s->cost[burm_stmt_NT] = c ;
    s->rule.burm_stmt = 2;
  }
}

static void burm_closure_ptr(struct burm_state *s, COST c) {
  if(burm_cost_code(&c,13,s) && COST_LESS(c,s->cost[burm_mem_NT])) {
burm_trace(burm_np, 13, c);     s->cost[burm_mem_NT] = c ;
    s->rule.burm_mem = 2;
    burm_closure_mem(s, c );
  }
}

static void burm_closure_cond(struct burm_state *s, COST c) {
  if(burm_cost_code(&c,33,s) && COST_LESS(c,s->cost[burm_stmt_NT])) {
burm_trace(burm_np, 33, c);     s->cost[burm_stmt_NT] = c ;
    s->rule.burm_stmt = 8;
  }
}

struct burm_state *burm_alloc_state(NODEPTR u,int op,int arity)
{
  struct burm_state *p, **k;
  p = (struct burm_state *)ALLOC(sizeof *p);
  burm_assert(p, PANIC("1:ALLOC returned NULL in burm_alloc_state\n"));
    burm_np = u;
  p->op = op;
  p->node = u;
  if(arity){
    k=(struct burm_state **)ALLOC(arity*sizeof (struct burm_state *));
    burm_assert(k, PANIC("2:ALLOC returned NULL in burm_alloc_state\n"));
    p->kids=k;
  }else
    p->kids=0;
  p->rule.burm_stmt =
  p->rule.burm_value =
  p->rule.burm_ri =
  p->rule.burm_rm =
  p->rule.burm_reg =
  p->rule.burm_imm =
  p->rule.burm_mem =
  p->rule.burm_ptr =
  p->rule.burm_cond =
  p->rule.burm_label =
  p->rule.burm_args =
    0;
  p->cost[1] =
  p->cost[2] =
  p->cost[3] =
  p->cost[4] =
  p->cost[5] =
  p->cost[6] =
  p->cost[7] =
  p->cost[8] =
  p->cost[9] =
  p->cost[10] =
  p->cost[11] =
    COST_INFINITY;
  return p;
}
struct burm_state *burm_label1(NODEPTR u) {
  int op, arity, i, immed_matched=0;
  COST c=COST_ZERO;
  struct burm_state *s,**k;
  NODEPTR *children;
  op=OP_LABEL(u);
  arity=burm_arity[op];
  switch(op){
  case 0:		/* DUMMY */
#ifdef LEAF_TRAP
    if(s=LEAF_TRAP(u,op))
      return s;
#endif
    s=burm_alloc_state(u,op,arity);
    SET_STATE(u,s);
    k=0;
    break;
  case 1:		/* Ret */
    s=burm_alloc_state(u,op,arity);
    SET_STATE(u,s);
    k=s->kids;
    children=GET_KIDS(u);
    for(i=0;i<arity;i++)
      k[i]=burm_label1(children[i]);
    if (   /* stmt: Ret(reg) */
      k[0]->rule.burm_reg
    ) {
      if(burm_cost_code(&c,16,s) && COST_LESS(c,s->cost[burm_stmt_NT])) {
burm_trace(burm_np, 16, c);         s->cost[burm_stmt_NT] = c ;
        s->rule.burm_stmt = 4;
      }
    }
    if (   /* stmt: Ret(imm) */
      k[0]->rule.burm_imm
    ) {
      if(burm_cost_code(&c,15,s) && COST_LESS(c,s->cost[burm_stmt_NT])) {
burm_trace(burm_np, 15, c);         s->cost[burm_stmt_NT] = c ;
        s->rule.burm_stmt = 3;
      }
    }
    break;
  case 2:		/* Br */
    s=burm_alloc_state(u,op,arity);
    SET_STATE(u,s);
    k=s->kids;
    children=GET_KIDS(u);
    for(i=0;i<arity;i++)
      k[i]=burm_label1(children[i]);
    if (   /* stmt: Br(cond,label,label) */
      k[0]->rule.burm_cond && 
      k[1]->rule.burm_label && 
      k[2]->rule.burm_label
    ) {
      if(burm_cost_code(&c,38,s) && COST_LESS(c,s->cost[burm_stmt_NT])) {
burm_trace(burm_np, 38, c);         s->cost[burm_stmt_NT] = c ;
        s->rule.burm_stmt = 10;
      }
    }
    if (   /* stmt: Br(label,DUMMY,DUMMY) */
      k[0]->rule.burm_label && 
      k[1]->op == 0 && 	/* DUMMY */
      k[2]->op == 0	/* DUMMY */
    ) {
      if(burm_cost_code(&c,37,s) && COST_LESS(c,s->cost[burm_stmt_NT])) {
burm_trace(burm_np, 37, c);         s->cost[burm_stmt_NT] = c ;
        s->rule.burm_stmt = 9;
      }
    }
    break;
  case 3:		/* Switch */
    s=burm_alloc_state(u,op,arity);
    SET_STATE(u,s);
    k=s->kids;
    children=GET_KIDS(u);
    for(i=0;i<arity;i++)
      k[i]=burm_label1(children[i]);
    break;
  case 4:		/* IndirectBr */
    s=burm_alloc_state(u,op,arity);
    SET_STATE(u,s);
    k=s->kids;
    children=GET_KIDS(u);
    for(i=0;i<arity;i++)
      k[i]=burm_label1(children[i]);
    break;
  case 5:		/* Invoke */
    s=burm_alloc_state(u,op,arity);
    SET_STATE(u,s);
    k=s->kids;
    children=GET_KIDS(u);
    for(i=0;i<arity;i++)
      k[i]=burm_label1(children[i]);
    break;
  case 6:		/* Resume */
    s=burm_alloc_state(u,op,arity);
    SET_STATE(u,s);
    k=s->kids;
    children=GET_KIDS(u);
    for(i=0;i<arity;i++)
      k[i]=burm_label1(children[i]);
    break;
  case 7:		/* Unreachable */
    s=burm_alloc_state(u,op,arity);
    SET_STATE(u,s);
    k=s->kids;
    children=GET_KIDS(u);
    for(i=0;i<arity;i++)
      k[i]=burm_label1(children[i]);
    break;
  case 8:		/* CleanupRet */
    s=burm_alloc_state(u,op,arity);
    SET_STATE(u,s);
    k=s->kids;
    children=GET_KIDS(u);
    for(i=0;i<arity;i++)
      k[i]=burm_label1(children[i]);
    break;
  case 9:		/* CatchRet */
    s=burm_alloc_state(u,op,arity);
    SET_STATE(u,s);
    k=s->kids;
    children=GET_KIDS(u);
    for(i=0;i<arity;i++)
      k[i]=burm_label1(children[i]);
    break;
  case 10:		/* CatchSwitch */
    s=burm_alloc_state(u,op,arity);
    SET_STATE(u,s);
    k=s->kids;
    children=GET_KIDS(u);
    for(i=0;i<arity;i++)
      k[i]=burm_label1(children[i]);
    break;
  case 11:		/* Add */
    s=burm_alloc_state(u,op,arity);
    SET_STATE(u,s);
    k=s->kids;
    children=GET_KIDS(u);
    for(i=0;i<arity;i++)
      k[i]=burm_label1(children[i]);
    if (   /* reg: Add(ri,reg) */
      k[0]->rule.burm_ri && 
      k[1]->rule.burm_reg
    ) {
      if(burm_cost_code(&c,25,s) && COST_LESS(c,s->cost[burm_reg_NT])) {
burm_trace(burm_np, 25, c);         s->cost[burm_reg_NT] = c ;
        s->rule.burm_reg = 6;
        burm_closure_reg(s, c );
      }
    }
    if (   /* reg: Add(reg,ri) */
      k[0]->rule.burm_reg && 
      k[1]->rule.burm_ri
    ) {
      if(burm_cost_code(&c,24,s) && COST_LESS(c,s->cost[burm_reg_NT])) {
burm_trace(burm_np, 24, c);         s->cost[burm_reg_NT] = c ;
        s->rule.burm_reg = 5;
        burm_closure_reg(s, c );
      }
    }
    break;
  case 12:		/* FAdd */
    s=burm_alloc_state(u,op,arity);
    SET_STATE(u,s);
    k=s->kids;
    children=GET_KIDS(u);
    for(i=0;i<arity;i++)
      k[i]=burm_label1(children[i]);
    break;
  case 13:		/* Sub */
    s=burm_alloc_state(u,op,arity);
    SET_STATE(u,s);
    k=s->kids;
    children=GET_KIDS(u);
    for(i=0;i<arity;i++)
      k[i]=burm_label1(children[i]);
    if (   /* reg: Sub(reg,ri) */
      k[0]->rule.burm_reg && 
      k[1]->rule.burm_ri
    ) {
      if(burm_cost_code(&c,26,s) && COST_LESS(c,s->cost[burm_reg_NT])) {
burm_trace(burm_np, 26, c);         s->cost[burm_reg_NT] = c ;
        s->rule.burm_reg = 7;
        burm_closure_reg(s, c );
      }
    }
    break;
  case 14:		/* FSub */
    s=burm_alloc_state(u,op,arity);
    SET_STATE(u,s);
    k=s->kids;
    children=GET_KIDS(u);
    for(i=0;i<arity;i++)
      k[i]=burm_label1(children[i]);
    break;
  case 15:		/* Mul */
    s=burm_alloc_state(u,op,arity);
    SET_STATE(u,s);
    k=s->kids;
    children=GET_KIDS(u);
    for(i=0;i<arity;i++)
      k[i]=burm_label1(children[i]);
    if (   /* reg: Mul(ri,reg) */
      k[0]->rule.burm_ri && 
      k[1]->rule.burm_reg
    ) {
      if(burm_cost_code(&c,28,s) && COST_LESS(c,s->cost[burm_reg_NT])) {
burm_trace(burm_np, 28, c);         s->cost[burm_reg_NT] = c ;
        s->rule.burm_reg = 9;
        burm_closure_reg(s, c );
      }
    }
    if (   /* reg: Mul(reg,ri) */
      k[0]->rule.burm_reg && 
      k[1]->rule.burm_ri
    ) {
      if(burm_cost_code(&c,27,s) && COST_LESS(c,s->cost[burm_reg_NT])) {
burm_trace(burm_np, 27, c);         s->cost[burm_reg_NT] = c ;
        s->rule.burm_reg = 8;
        burm_closure_reg(s, c );
      }
    }
    break;
  case 16:		/* FMul */
    s=burm_alloc_state(u,op,arity);
    SET_STATE(u,s);
    k=s->kids;
    children=GET_KIDS(u);
    for(i=0;i<arity;i++)
      k[i]=burm_label1(children[i]);
    break;
  case 17:		/* UDiv */
    s=burm_alloc_state(u,op,arity);
    SET_STATE(u,s);
    k=s->kids;
    children=GET_KIDS(u);
    for(i=0;i<arity;i++)
      k[i]=burm_label1(children[i]);
    if (   /* reg: UDiv(rm,imm) */
      k[0]->rule.burm_rm && 
      k[1]->rule.burm_imm
    ) {
      if(burm_cost_code(&c,31,s) && COST_LESS(c,s->cost[burm_reg_NT])) {
burm_trace(burm_np, 31, c);         s->cost[burm_reg_NT] = c ;
        s->rule.burm_reg = 12;
        burm_closure_reg(s, c );
      }
    }
    if (   /* reg: UDiv(rm,rm) */
      k[0]->rule.burm_rm && 
      k[1]->rule.burm_rm
    ) {
      if(burm_cost_code(&c,29,s) && COST_LESS(c,s->cost[burm_reg_NT])) {
burm_trace(burm_np, 29, c);         s->cost[burm_reg_NT] = c ;
        s->rule.burm_reg = 10;
        burm_closure_reg(s, c );
      }
    }
    break;
  case 18:		/* SDiv */
    s=burm_alloc_state(u,op,arity);
    SET_STATE(u,s);
    k=s->kids;
    children=GET_KIDS(u);
    for(i=0;i<arity;i++)
      k[i]=burm_label1(children[i]);
    if (   /* reg: SDiv(rm,imm) */
      k[0]->rule.burm_rm && 
      k[1]->rule.burm_imm
    ) {
      if(burm_cost_code(&c,32,s) && COST_LESS(c,s->cost[burm_reg_NT])) {
burm_trace(burm_np, 32, c);         s->cost[burm_reg_NT] = c ;
        s->rule.burm_reg = 13;
        burm_closure_reg(s, c );
      }
    }
    if (   /* reg: SDiv(rm,rm) */
      k[0]->rule.burm_rm && 
      k[1]->rule.burm_rm
    ) {
      if(burm_cost_code(&c,30,s) && COST_LESS(c,s->cost[burm_reg_NT])) {
burm_trace(burm_np, 30, c);         s->cost[burm_reg_NT] = c ;
        s->rule.burm_reg = 11;
        burm_closure_reg(s, c );
      }
    }
    break;
  case 19:		/* FDiv */
    s=burm_alloc_state(u,op,arity);
    SET_STATE(u,s);
    k=s->kids;
    children=GET_KIDS(u);
    for(i=0;i<arity;i++)
      k[i]=burm_label1(children[i]);
    break;
  case 20:		/* URem */
    s=burm_alloc_state(u,op,arity);
    SET_STATE(u,s);
    k=s->kids;
    children=GET_KIDS(u);
    for(i=0;i<arity;i++)
      k[i]=burm_label1(children[i]);
    break;
  case 21:		/* SRem */
    s=burm_alloc_state(u,op,arity);
    SET_STATE(u,s);
    k=s->kids;
    children=GET_KIDS(u);
    for(i=0;i<arity;i++)
      k[i]=burm_label1(children[i]);
    break;
  case 22:		/* FRem */
    s=burm_alloc_state(u,op,arity);
    SET_STATE(u,s);
    k=s->kids;
    children=GET_KIDS(u);
    for(i=0;i<arity;i++)
      k[i]=burm_label1(children[i]);
    break;
  case 23:		/* Shl */
    s=burm_alloc_state(u,op,arity);
    SET_STATE(u,s);
    k=s->kids;
    children=GET_KIDS(u);
    for(i=0;i<arity;i++)
      k[i]=burm_label1(children[i]);
    break;
  case 24:		/* LShr */
    s=burm_alloc_state(u,op,arity);
    SET_STATE(u,s);
    k=s->kids;
    children=GET_KIDS(u);
    for(i=0;i<arity;i++)
      k[i]=burm_label1(children[i]);
    break;
  case 25:		/* AShr */
    s=burm_alloc_state(u,op,arity);
    SET_STATE(u,s);
    k=s->kids;
    children=GET_KIDS(u);
    for(i=0;i<arity;i++)
      k[i]=burm_label1(children[i]);
    break;
  case 26:		/* And */
    s=burm_alloc_state(u,op,arity);
    SET_STATE(u,s);
    k=s->kids;
    children=GET_KIDS(u);
    for(i=0;i<arity;i++)
      k[i]=burm_label1(children[i]);
    break;
  case 27:		/* Or */
    s=burm_alloc_state(u,op,arity);
    SET_STATE(u,s);
    k=s->kids;
    children=GET_KIDS(u);
    for(i=0;i<arity;i++)
      k[i]=burm_label1(children[i]);
    break;
  case 28:		/* Xor */
    s=burm_alloc_state(u,op,arity);
    SET_STATE(u,s);
    k=s->kids;
    children=GET_KIDS(u);
    for(i=0;i<arity;i++)
      k[i]=burm_label1(children[i]);
    break;
  case 29:		/* Alloca */
    s=burm_alloc_state(u,op,arity);
    SET_STATE(u,s);
    k=s->kids;
    children=GET_KIDS(u);
    for(i=0;i<arity;i++)
      k[i]=burm_label1(children[i]);
    if (   /* mem: Alloca(imm) */
      k[0]->rule.burm_imm
    ) {
      if(burm_cost_code(&c,17,s) && COST_LESS(c,s->cost[burm_mem_NT])) {
burm_trace(burm_np, 17, c);         s->cost[burm_mem_NT] = c ;
        s->rule.burm_mem = 3;
        burm_closure_mem(s, c );
      }
    }
    break;
  case 30:		/* Load */
    s=burm_alloc_state(u,op,arity);
    SET_STATE(u,s);
    k=s->kids;
    children=GET_KIDS(u);
    for(i=0;i<arity;i++)
      k[i]=burm_label1(children[i]);
    if (   /* reg: Load(mem) */
      k[0]->rule.burm_mem
    ) {
      if(burm_cost_code(&c,20,s) && COST_LESS(c,s->cost[burm_reg_NT])) {
burm_trace(burm_np, 20, c);         s->cost[burm_reg_NT] = c ;
        s->rule.burm_reg = 4;
        burm_closure_reg(s, c );
      }
    }
    if (   /* reg: Load(imm) */
      k[0]->rule.burm_imm
    ) {
      if(burm_cost_code(&c,19,s) && COST_LESS(c,s->cost[burm_reg_NT])) {
burm_trace(burm_np, 19, c);         s->cost[burm_reg_NT] = c ;
        s->rule.burm_reg = 3;
        burm_closure_reg(s, c );
      }
    }
    if (   /* reg: Load(reg) */
      k[0]->rule.burm_reg
    ) {
      if(burm_cost_code(&c,18,s) && COST_LESS(c,s->cost[burm_reg_NT])) {
burm_trace(burm_np, 18, c);         s->cost[burm_reg_NT] = c ;
        s->rule.burm_reg = 2;
        burm_closure_reg(s, c );
      }
    }
    if (   /* imm: Load(IMM) */
      k[0]->op == 66	/* IMM */
    ) {
      if(burm_cost_code(&c,10,s) && COST_LESS(c,s->cost[burm_imm_NT])) {
burm_trace(burm_np, 10, c);         s->cost[burm_imm_NT] = c ;
        s->rule.burm_imm = 1;
        burm_closure_imm(s, c );
      }
    }
    break;
  case 31:		/* Store */
    s=burm_alloc_state(u,op,arity);
    SET_STATE(u,s);
    k=s->kids;
    children=GET_KIDS(u);
    for(i=0;i<arity;i++)
      k[i]=burm_label1(children[i]);
    if (   /* stmt: Store(mem,mem) */
      k[0]->rule.burm_mem && 
      k[1]->rule.burm_mem
    ) {
      if(burm_cost_code(&c,23,s) && COST_LESS(c,s->cost[burm_stmt_NT])) {
burm_trace(burm_np, 23, c);         s->cost[burm_stmt_NT] = c ;
        s->rule.burm_stmt = 7;
      }
    }
    if (   /* stmt: Store(reg,mem) */
      k[0]->rule.burm_reg && 
      k[1]->rule.burm_mem
    ) {
      if(burm_cost_code(&c,22,s) && COST_LESS(c,s->cost[burm_stmt_NT])) {
burm_trace(burm_np, 22, c);         s->cost[burm_stmt_NT] = c ;
        s->rule.burm_stmt = 6;
      }
    }
    if (   /* stmt: Store(imm,mem) */
      k[0]->rule.burm_imm && 
      k[1]->rule.burm_mem
    ) {
      if(burm_cost_code(&c,21,s) && COST_LESS(c,s->cost[burm_stmt_NT])) {
burm_trace(burm_np, 21, c);         s->cost[burm_stmt_NT] = c ;
        s->rule.burm_stmt = 5;
      }
    }
    break;
  case 32:		/* GetElementPtr */
    s=burm_alloc_state(u,op,arity);
    SET_STATE(u,s);
    k=s->kids;
    children=GET_KIDS(u);
    for(i=0;i<arity;i++)
      k[i]=burm_label1(children[i]);
    if (   /* ptr: GetElementPtr(mem,ri,imm) */
      k[0]->rule.burm_mem && 
      k[1]->rule.burm_ri && 
      k[2]->rule.burm_imm
    ) {
      if(burm_cost_code(&c,46,s) && COST_LESS(c,s->cost[burm_ptr_NT])) {
burm_trace(burm_np, 46, c);         s->cost[burm_ptr_NT] = c ;
        s->rule.burm_ptr = 2;
        burm_closure_ptr(s, c );
      }
    }
    if (   /* ptr: GetElementPtr(ri,ri,imm) */
      k[0]->rule.burm_ri && 
      k[1]->rule.burm_ri && 
      k[2]->rule.burm_imm
    ) {
      if(burm_cost_code(&c,45,s) && COST_LESS(c,s->cost[burm_ptr_NT])) {
burm_trace(burm_np, 45, c);         s->cost[burm_ptr_NT] = c ;
        s->rule.burm_ptr = 1;
        burm_closure_ptr(s, c );
      }
    }
    break;
  case 33:		/* Fence */
    s=burm_alloc_state(u,op,arity);
    SET_STATE(u,s);
    k=s->kids;
    children=GET_KIDS(u);
    for(i=0;i<arity;i++)
      k[i]=burm_label1(children[i]);
    break;
  case 34:		/* AtomicCmpXchg */
    s=burm_alloc_state(u,op,arity);
    SET_STATE(u,s);
    k=s->kids;
    children=GET_KIDS(u);
    for(i=0;i<arity;i++)
      k[i]=burm_label1(children[i]);
    break;
  case 35:		/* AtomicRMW */
    s=burm_alloc_state(u,op,arity);
    SET_STATE(u,s);
    k=s->kids;
    children=GET_KIDS(u);
    for(i=0;i<arity;i++)
      k[i]=burm_label1(children[i]);
    break;
  case 36:		/* Trunc */
    s=burm_alloc_state(u,op,arity);
    SET_STATE(u,s);
    k=s->kids;
    children=GET_KIDS(u);
    for(i=0;i<arity;i++)
      k[i]=burm_label1(children[i]);
    if (   /* reg: Trunc(reg) */
      k[0]->rule.burm_reg
    ) {
      if(burm_cost_code(&c,42,s) && COST_LESS(c,s->cost[burm_reg_NT])) {
burm_trace(burm_np, 42, c);         s->cost[burm_reg_NT] = c ;
        s->rule.burm_reg = 15;
        burm_closure_reg(s, c );
      }
    }
    break;
  case 37:		/* ZExt */
    s=burm_alloc_state(u,op,arity);
    SET_STATE(u,s);
    k=s->kids;
    children=GET_KIDS(u);
    for(i=0;i<arity;i++)
      k[i]=burm_label1(children[i]);
    break;
  case 38:		/* SExt */
    s=burm_alloc_state(u,op,arity);
    SET_STATE(u,s);
    k=s->kids;
    children=GET_KIDS(u);
    for(i=0;i<arity;i++)
      k[i]=burm_label1(children[i]);
    break;
  case 39:		/* FPToUI */
    s=burm_alloc_state(u,op,arity);
    SET_STATE(u,s);
    k=s->kids;
    children=GET_KIDS(u);
    for(i=0;i<arity;i++)
      k[i]=burm_label1(children[i]);
    break;
  case 40:		/* FPToSI */
    s=burm_alloc_state(u,op,arity);
    SET_STATE(u,s);
    k=s->kids;
    children=GET_KIDS(u);
    for(i=0;i<arity;i++)
      k[i]=burm_label1(children[i]);
    break;
  case 41:		/* UIToFP */
    s=burm_alloc_state(u,op,arity);
    SET_STATE(u,s);
    k=s->kids;
    children=GET_KIDS(u);
    for(i=0;i<arity;i++)
      k[i]=burm_label1(children[i]);
    break;
  case 42:		/* SIToFP */
    s=burm_alloc_state(u,op,arity);
    SET_STATE(u,s);
    k=s->kids;
    children=GET_KIDS(u);
    for(i=0;i<arity;i++)
      k[i]=burm_label1(children[i]);
    break;
  case 43:		/* FPTrunc */
    s=burm_alloc_state(u,op,arity);
    SET_STATE(u,s);
    k=s->kids;
    children=GET_KIDS(u);
    for(i=0;i<arity;i++)
      k[i]=burm_label1(children[i]);
    break;
  case 44:		/* FPExt */
    s=burm_alloc_state(u,op,arity);
    SET_STATE(u,s);
    k=s->kids;
    children=GET_KIDS(u);
    for(i=0;i<arity;i++)
      k[i]=burm_label1(children[i]);
    break;
  case 45:		/* PtrToInt */
    s=burm_alloc_state(u,op,arity);
    SET_STATE(u,s);
    k=s->kids;
    children=GET_KIDS(u);
    for(i=0;i<arity;i++)
      k[i]=burm_label1(children[i]);
    break;
  case 46:		/* IntToPtr */
    s=burm_alloc_state(u,op,arity);
    SET_STATE(u,s);
    k=s->kids;
    children=GET_KIDS(u);
    for(i=0;i<arity;i++)
      k[i]=burm_label1(children[i]);
    break;
  case 47:		/* BitCast */
    s=burm_alloc_state(u,op,arity);
    SET_STATE(u,s);
    k=s->kids;
    children=GET_KIDS(u);
    for(i=0;i<arity;i++)
      k[i]=burm_label1(children[i]);
    if (   /* reg: BitCast(mem) */
      k[0]->rule.burm_mem
    ) {
      if(burm_cost_code(&c,44,s) && COST_LESS(c,s->cost[burm_reg_NT])) {
burm_trace(burm_np, 44, c);         s->cost[burm_reg_NT] = c ;
        s->rule.burm_reg = 17;
        burm_closure_reg(s, c );
      }
    }
    if (   /* reg: BitCast(reg) */
      k[0]->rule.burm_reg
    ) {
      if(burm_cost_code(&c,43,s) && COST_LESS(c,s->cost[burm_reg_NT])) {
burm_trace(burm_np, 43, c);         s->cost[burm_reg_NT] = c ;
        s->rule.burm_reg = 16;
        burm_closure_reg(s, c );
      }
    }
    break;
  case 48:		/* AddrSpaceCast */
    s=burm_alloc_state(u,op,arity);
    SET_STATE(u,s);
    k=s->kids;
    children=GET_KIDS(u);
    for(i=0;i<arity;i++)
      k[i]=burm_label1(children[i]);
    break;
  case 49:		/* CleanupPad */
    s=burm_alloc_state(u,op,arity);
    SET_STATE(u,s);
    k=s->kids;
    children=GET_KIDS(u);
    for(i=0;i<arity;i++)
      k[i]=burm_label1(children[i]);
    break;
  case 50:		/* CatchPad */
    s=burm_alloc_state(u,op,arity);
    SET_STATE(u,s);
    k=s->kids;
    children=GET_KIDS(u);
    for(i=0;i<arity;i++)
      k[i]=burm_label1(children[i]);
    break;
  case 51:		/* ICmp */
    s=burm_alloc_state(u,op,arity);
    SET_STATE(u,s);
    k=s->kids;
    children=GET_KIDS(u);
    for(i=0;i<arity;i++)
      k[i]=burm_label1(children[i]);
    if (   /* cond: ICmp(reg,reg) */
      k[0]->rule.burm_reg && 
      k[1]->rule.burm_reg
    ) {
      if(burm_cost_code(&c,36,s) && COST_LESS(c,s->cost[burm_cond_NT])) {
burm_trace(burm_np, 36, c);         s->cost[burm_cond_NT] = c ;
        s->rule.burm_cond = 3;
        burm_closure_cond(s, c );
      }
    }
    if (   /* cond: ICmp(reg,imm) */
      k[0]->rule.burm_reg && 
      k[1]->rule.burm_imm
    ) {
      if(burm_cost_code(&c,35,s) && COST_LESS(c,s->cost[burm_cond_NT])) {
burm_trace(burm_np, 35, c);         s->cost[burm_cond_NT] = c ;
        s->rule.burm_cond = 2;
        burm_closure_cond(s, c );
      }
    }
    if (   /* cond: ICmp(imm,imm) */
      k[0]->rule.burm_imm && 
      k[1]->rule.burm_imm
    ) {
      if(burm_cost_code(&c,34,s) && COST_LESS(c,s->cost[burm_cond_NT])) {
burm_trace(burm_np, 34, c);         s->cost[burm_cond_NT] = c ;
        s->rule.burm_cond = 1;
        burm_closure_cond(s, c );
      }
    }
    break;
  case 52:		/* FCmp */
    s=burm_alloc_state(u,op,arity);
    SET_STATE(u,s);
    k=s->kids;
    children=GET_KIDS(u);
    for(i=0;i<arity;i++)
      k[i]=burm_label1(children[i]);
    break;
  case 53:		/* PHI */
    s=burm_alloc_state(u,op,arity);
    SET_STATE(u,s);
    k=s->kids;
    children=GET_KIDS(u);
    for(i=0;i<arity;i++)
      k[i]=burm_label1(children[i]);
    break;
  case 54:		/* Call */
    s=burm_alloc_state(u,op,arity);
    SET_STATE(u,s);
    k=s->kids;
    children=GET_KIDS(u);
    for(i=0;i<arity;i++)
      k[i]=burm_label1(children[i]);
    if (   /* reg: Call(args) */
      k[0]->rule.burm_args
    ) {
      if(burm_cost_code(&c,39,s) && COST_LESS(c,s->cost[burm_reg_NT])) {
burm_trace(burm_np, 39, c);         s->cost[burm_reg_NT] = c ;
        s->rule.burm_reg = 14;
        burm_closure_reg(s, c );
      }
    }
    break;
  case 55:		/* Select */
    s=burm_alloc_state(u,op,arity);
    SET_STATE(u,s);
    k=s->kids;
    children=GET_KIDS(u);
    for(i=0;i<arity;i++)
      k[i]=burm_label1(children[i]);
    break;
  case 56:		/* UserOp1 */
    s=burm_alloc_state(u,op,arity);
    SET_STATE(u,s);
    k=s->kids;
    children=GET_KIDS(u);
    for(i=0;i<arity;i++)
      k[i]=burm_label1(children[i]);
    break;
  case 57:		/* UserOp2 */
    s=burm_alloc_state(u,op,arity);
    SET_STATE(u,s);
    k=s->kids;
    children=GET_KIDS(u);
    for(i=0;i<arity;i++)
      k[i]=burm_label1(children[i]);
    break;
  case 58:		/* VAArg */
    s=burm_alloc_state(u,op,arity);
    SET_STATE(u,s);
    k=s->kids;
    children=GET_KIDS(u);
    for(i=0;i<arity;i++)
      k[i]=burm_label1(children[i]);
    break;
  case 59:		/* ExtractElement */
    s=burm_alloc_state(u,op,arity);
    SET_STATE(u,s);
    k=s->kids;
    children=GET_KIDS(u);
    for(i=0;i<arity;i++)
      k[i]=burm_label1(children[i]);
    break;
  case 60:		/* InsertElement */
    s=burm_alloc_state(u,op,arity);
    SET_STATE(u,s);
    k=s->kids;
    children=GET_KIDS(u);
    for(i=0;i<arity;i++)
      k[i]=burm_label1(children[i]);
    break;
  case 61:		/* ShuffleVector */
    s=burm_alloc_state(u,op,arity);
    SET_STATE(u,s);
    k=s->kids;
    children=GET_KIDS(u);
    for(i=0;i<arity;i++)
      k[i]=burm_label1(children[i]);
    break;
  case 62:		/* ExtractValue */
    s=burm_alloc_state(u,op,arity);
    SET_STATE(u,s);
    k=s->kids;
    children=GET_KIDS(u);
    for(i=0;i<arity;i++)
      k[i]=burm_label1(children[i]);
    break;
  case 63:		/* InsertValue */
    s=burm_alloc_state(u,op,arity);
    SET_STATE(u,s);
    k=s->kids;
    children=GET_KIDS(u);
    for(i=0;i<arity;i++)
      k[i]=burm_label1(children[i]);
    break;
  case 64:		/* LandingPad */
    s=burm_alloc_state(u,op,arity);
    SET_STATE(u,s);
    k=s->kids;
    children=GET_KIDS(u);
    for(i=0;i<arity;i++)
      k[i]=burm_label1(children[i]);
    break;
  case 65:		/* REG */
#ifdef LEAF_TRAP
    if(s=LEAF_TRAP(u,op))
      return s;
#endif
    s=burm_alloc_state(u,op,arity);
    SET_STATE(u,s);
    k=0;
    {  		/* reg: REG */
      if(burm_cost_code(&c,9,s) && COST_LESS(c,s->cost[burm_reg_NT])) {
burm_trace(burm_np, 9, c);         s->cost[burm_reg_NT] = c ;
        s->rule.burm_reg = 1;
        burm_closure_reg(s, c );
      }
    }
    break;
  case 66:		/* IMM */
#ifdef LEAF_TRAP
    if(s=LEAF_TRAP(u,op))
      return s;
#endif
    s=burm_alloc_state(u,op,arity);
    SET_STATE(u,s);
    k=0;
    {  		/* imm: IMM */
      if(burm_cost_code(&c,11,s) && COST_LESS(c,s->cost[burm_imm_NT])) {
burm_trace(burm_np, 11, c);         s->cost[burm_imm_NT] = c ;
        s->rule.burm_imm = 2;
        burm_closure_imm(s, c );
      }
    }
    break;
  case 67:		/* MEM */
#ifdef LEAF_TRAP
    if(s=LEAF_TRAP(u,op))
      return s;
#endif
    s=burm_alloc_state(u,op,arity);
    SET_STATE(u,s);
    k=0;
    {  		/* mem: MEM */
      if(burm_cost_code(&c,12,s) && COST_LESS(c,s->cost[burm_mem_NT])) {
burm_trace(burm_np, 12, c);         s->cost[burm_mem_NT] = c ;
        s->rule.burm_mem = 1;
        burm_closure_mem(s, c );
      }
    }
    break;
  case 68:		/* LABEL */
#ifdef LEAF_TRAP
    if(s=LEAF_TRAP(u,op))
      return s;
#endif
    s=burm_alloc_state(u,op,arity);
    SET_STATE(u,s);
    k=0;
    {  		/* label: LABEL */
      if(burm_cost_code(&c,14,s) && COST_LESS(c,s->cost[burm_label_NT])) {
burm_trace(burm_np, 14, c);         s->cost[burm_label_NT] = c ;
        s->rule.burm_label = 1;
      }
    }
    break;
  case 69:		/* ARGS */
    s=burm_alloc_state(u,op,arity);
    SET_STATE(u,s);
    k=s->kids;
    children=GET_KIDS(u);
    for(i=0;i<arity;i++)
      k[i]=burm_label1(children[i]);
    if (   /* args: ARGS(value,args) */
      k[0]->rule.burm_value && 
      k[1]->rule.burm_args
    ) {
      if(burm_cost_code(&c,41,s) && COST_LESS(c,s->cost[burm_args_NT])) {
burm_trace(burm_np, 41, c);         s->cost[burm_args_NT] = c ;
        s->rule.burm_args = 2;
      }
    }
    break;
  case 70:		/* NOARGS */
#ifdef LEAF_TRAP
    if(s=LEAF_TRAP(u,op))
      return s;
#endif
    s=burm_alloc_state(u,op,arity);
    SET_STATE(u,s);
    k=0;
    {  		/* args: NOARGS */
      if(burm_cost_code(&c,40,s) && COST_LESS(c,s->cost[burm_args_NT])) {
burm_trace(burm_np, 40, c);         s->cost[burm_args_NT] = c ;
        s->rule.burm_args = 1;
      }
    }
    break;
  case 71:		/* PTR */
    s=burm_alloc_state(u,op,arity);
    SET_STATE(u,s);
    k=s->kids;
    children=GET_KIDS(u);
    for(i=0;i<arity;i++)
      k[i]=burm_label1(children[i]);
    break;
  default:
    burm_assert(0, PANIC("Bad operator %d in burm_state\n", op));
  }
  return s;
}

struct burm_state *burm_label(NODEPTR p) {
  burm_label1(p);
  return ((struct burm_state *)STATE_LABEL(p))->rule.burm_stmt ? STATE_LABEL(p) : 0;
}

void burm_free(struct burm_state *s)
{
  int i,arity=burm_arity[s->op];
  if(s->kids==0)
    free(s);
  else {
    for(i=0;i<arity;i++)
      burm_free(s->kids[i]);
    free(s->kids);
  }
}
struct burm_state *burm_immed(struct burm_state *s,int n)
{
  NODEPTR *children = GET_KIDS(s->node);
  if(s->kids[n])
    return s->kids[n];
  else
  return s->kids[n]=burm_label1(children[n]);
}
int burm_op_label(NODEPTR p) {
  burm_assert(p, PANIC("NULL tree in burm_op_label\n"));
  return OP_LABEL(p);
}

struct burm_state *burm_state_label(NODEPTR p) {
  burm_assert(p, PANIC("NULL tree in burm_state_label\n"));
  return STATE_LABEL(p);
}

NODEPTR burm_child(NODEPTR p, int index) {
  NODEPTR *kids;
  burm_assert(p, PANIC("NULL tree in burm_child\n"));
  kids=GET_KIDS(p);
  burm_assert((0<=index && index<burm_arity[OP_LABEL(p)]),
    PANIC("Bad index %d in burm_child\n", index));

  return kids[index];
}
NODEPTR *burm_kids(NODEPTR p, int eruleno, NODEPTR kids[]) {
  burm_assert(p, PANIC("NULL tree in burm_kids\n"));
  burm_assert(kids, PANIC("NULL kids in burm_kids\n"));
  switch (eruleno) {
  case 33: /* stmt: cond */
  case 13: /* mem: ptr */
  case 8: /* rm: mem */
  case 7: /* rm: reg */
  case 6: /* ri: imm */
  case 5: /* ri: reg */
  case 4: /* value: mem */
  case 3: /* value: imm */
  case 2: /* value: reg */
  case 1: /* stmt: mem */
  case 0: /* stmt: reg */
    kids[0] = p;
    break;
  case 40: /* args: NOARGS */
  case 14: /* label: LABEL */
  case 12: /* mem: MEM */
  case 11: /* imm: IMM */
  case 10: /* imm: Load(IMM) */
  case 9: /* reg: REG */
    break;
  case 44: /* reg: BitCast(mem) */
  case 43: /* reg: BitCast(reg) */
  case 42: /* reg: Trunc(reg) */
  case 39: /* reg: Call(args) */
  case 37: /* stmt: Br(label,DUMMY,DUMMY) */
  case 20: /* reg: Load(mem) */
  case 19: /* reg: Load(imm) */
  case 18: /* reg: Load(reg) */
  case 17: /* mem: Alloca(imm) */
  case 16: /* stmt: Ret(reg) */
  case 15: /* stmt: Ret(imm) */
    kids[0] = burm_child(p,0);
    break;
  case 41: /* args: ARGS(value,args) */
  case 36: /* cond: ICmp(reg,reg) */
  case 35: /* cond: ICmp(reg,imm) */
  case 34: /* cond: ICmp(imm,imm) */
  case 32: /* reg: SDiv(rm,imm) */
  case 31: /* reg: UDiv(rm,imm) */
  case 30: /* reg: SDiv(rm,rm) */
  case 29: /* reg: UDiv(rm,rm) */
  case 28: /* reg: Mul(ri,reg) */
  case 27: /* reg: Mul(reg,ri) */
  case 26: /* reg: Sub(reg,ri) */
  case 25: /* reg: Add(ri,reg) */
  case 24: /* reg: Add(reg,ri) */
  case 23: /* stmt: Store(mem,mem) */
  case 22: /* stmt: Store(reg,mem) */
  case 21: /* stmt: Store(imm,mem) */
    kids[0] = burm_child(p,0);
    kids[1] = burm_child(p,1);
    break;
  case 46: /* ptr: GetElementPtr(mem,ri,imm) */
  case 45: /* ptr: GetElementPtr(ri,ri,imm) */
  case 38: /* stmt: Br(cond,label,label) */
    kids[0] = burm_child(p,0);
    kids[1] = burm_child(p,1);
    kids[2] = burm_child(p,2);
    break;
  default:
    burm_assert(0, PANIC("Bad external rule number %d in burm_kids\n", eruleno));
  }
  return kids;
}

void dumpCover(NODEPTR p, int goalnt, int indent)
{
  int eruleno = burm_rule(STATE_LABEL(p), goalnt);
  short *nts = burm_nts[eruleno];
  NODEPTR kids[10];
  int i;

  std::cerr << "\t\t";
  for (i = 0; i < indent; i++)
    std::cerr << " ";
  std::cerr << burm_string[eruleno] << "\n";
  burm_kids(p, eruleno, kids);
  for (i = 0; nts[i]; i++)
    dumpCover(kids[i], nts[i], indent + 1);
}


#pragma GCC diagnostic pop


#include "llc_olive_helper.cxx"
