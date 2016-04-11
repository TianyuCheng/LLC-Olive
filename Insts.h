#ifndef INSTS_H
#define INSTS_H

enum { Ret=1, Br=2, Switch=3, IndirectBr=4, Invoke=5, Resume=6, Unreachable=7, CleanupRet=8, CatchRet=9, CatchSwitch=10 };
enum { Add=11, FAdd=12, Sub=13, FSub=14, Mul=15, FMul=16, UDiv=17, SDiv=18, FDiv=19, URem=20, SRem=21, FRem=22 };
enum { Shl=23, LShr=24, AShr=25, And=26, Or=27, Xor=28 };
enum { Alloca=29, Load=30, Store=31, GetElementPtr=32, Fence=33, AtomicCmpXchg=34, AtomicRMW=35 };
enum { Trunc=36, ZExt=37, SExt=38, FPToUI=39, FPToSI=40, UIToFP=41, SIToFP=42, FPTrunc=43, FPExt=44, PtrToInt=45, IntToPtr=46, BitCast=47, AddrSpaceCast=48 };
enum { CleanupPad=49, CatchPad=50 };
enum { ICmp=51, FCmp=52, PHI=53, Call=54, Select=55, UserOp1=56, UserOp2=57, VAArg=58, ExtractElement=59, InsertElement=60, ShuffleVector=61, ExtractValue=62, InsertValue=63, LandingPad=64 };
enum { REG=65, IMM=66, MEM=67, LABEL=68, ARGS=69, NOARGS=70, PTR=71, GlobalValue=72, PHIInst=73, DUMMY=0 };

#endif /* end of include guard: INSTS_H */
