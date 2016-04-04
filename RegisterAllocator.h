#ifndef REGISTERALLOCATOR_H
#define REGISTERALLOCATOR_H

#include <iostream>
#include <map>
#include <set>
#include <queue>
#include "assert.h"
#include "LiveRange.h"

enum Register {
    RAX, RBX, RCX, RDX,
    RSI, RDI, RBP, RSP,
    R8, R9, R10, R11,
    R12, R13, R14, R15
};

// System V X86_64 's calling convention
static Register scratch_regs [] = { RAX, RDI, RSI, RDX, RCX, R8, R9, R10, R11 };
static Register preserve_regs [] = { RBX, RSP, RBP, R12, R13, R14, R15 };
static Register params_regs  [] = { RDI, RSI, RDX, RCX, R8, R9 };

static const char* registers[] = {
    "rax", "rbx", "rcx", "rdx",
    "rsi", "rdi", "rbp", "rsp",
    "r8", "r9", "r10", "r11", 
    "r12", "r13", "r14", "r15", 
};
static int NUM_REGS = 16;

/*
// use naive linear search to insert elem 
static void insert_active_set (std::vector<LiveRange*>& vec, LiveRange* elem) {
    vec.push_back(elem);
    for (int i = vec.size()-1; i >= 0 ; i --) {
        if (vec[i]->endpoint <= elem->endpoint) break;
        else {
            LiveRange* tmp = vec[i];
            vec[i] = vec[i+1];
            vec[i+1] = tmp;
        }
    }
}

// use naive linear search to insert elem 
static void insert_intervals (std::vector<LiveRange*>& vec, LiveRange* elem) {
    vec.push_back(elem);
    for (int i = vec.size()-1; i >= 0 ; i --) {
        if (vec[i]->startpoint <= elem->startpoint) break;
        else {
            LiveRange* tmp = vec[i];
            vec[i] = vec[i+1];
            vec[i+1] = tmp;
        }
    }
}
*/

/**
 * Register Allocator
 * */
class RegisterAllocator
{
public:
    RegisterAllocator(int num_regs) {
        // initialize register map
        // for (int i = 0; i < NUM_REGS; i++) 
        //    register_map[i] = NULL;
    }

    virtual ~RegisterAllocator() { }

    std::string Allocate() {
        return std::string("rax");
    }
    /*
    // naive version of linear scan
    void linearScanAllocate ();
    void expireOldIntervals (int i);
    void spillAtInterval (int i);
    */


    bool isIntersect (Interval* ia, Interval* ib);
    int findNextIntersect (int pos, Interval* cur_itv, Interval* itv);
    int findNextUseHelper(std::vector<int>& use_vec, int after);
    int findNextUse (int cur_iid, int iid);

    void updateRAState(int cur_iid);
    int tryAllocateFreeReg (int cur_iid);
    int allocateBlockedReg (int cur_iid);
    void resolveConflicts ();
    void linearScanSSA ();

    std::vector<int> get_virtual2machine() const {
        return virtual2machine;
    }

private:
    int num_regs;
    /*
    // restore active_set set of live interval in increasing order of endpoint
    std::vector<LiveRange*> active_set;
    // restore live all_intervals of all variables
    std::vector<LiveRange*> all_intervals;
    // restore the free registers
    std::map<int, LiveRange*> register_map;
    */
    std::set<int> active;
    std::set<int> inactive;
    std::set<int> handled;
    std::set<int> unhandled;
    std::vector<Interval*> all_intervals;
    std::map<int, std::vector<int>*> use_contexts;

    // gloally restore ssa form mapping
    std::vector<int> virtual2machine;
};

#endif /* end of include guard: REGISTERALLOCATOR_H */
