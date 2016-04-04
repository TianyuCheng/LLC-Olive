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
// static int NUM_REGS = 16;

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
        this->NUM_REGS = num_regs;
        // initialize register map
        for (int i = 0; i < NUM_REGS; i++) 
            register_map.insert(std::pair<int, int>(i, -1));
        // all_intervals.insert(ai.begin(), ai.end()); 
         // use_contexts.insert(uc.begin(), uc.end()); 
    }

    virtual ~RegisterAllocator() { }

    std::string Allocate() {
        // TODO: interaction 
        // return std::string("rax");
    }

    bool isIntersect (Interval* ia, Interval* ib);
    int findNextIntersect (int pos, Interval* cur_itv, Interval* itv);
    int findNextUseHelper(std::vector<int>& use_vec, int after);
    int findNextUse (int cur_iid, int iid);
    void splitInterval(int iid, int start, int reg);

    void updateRAState(int cur_iid);
    int tryAllocateFreeReg (int cur_iid);
    int allocateBlockedReg (int cur_iid);
    void resolveConflicts ();
    void linearScanSSA ();

    std::vector<int> get_virtual2machine() const {
        return virtual2machine;
    }

    void insert_intervals (Interval* interval) {
        all_intervals.push_back(interval);
        for (int i = all_intervals.size()-1; i > 0 ; i --) {
            int cur_startpoint = all_intervals[i]->liveranges[0].startpoint;
            int prev_startpoint = all_intervals[i-1]->liveranges[0].startpoint;
            if ( prev_startpoint <= cur_startpoint ) break;
            else {
                Interval* tmp = all_intervals[i];
                all_intervals[i] = all_intervals[i-1];
                all_intervals[i-1] = tmp;
            }
        }
    }

    void set_all_intervals(std::map<int, Interval*> &ai) {
        std::cout << "Startpoint of Built intervals" << std::endl;
        for (auto it=ai.rbegin(); it!=ai.rend(); it++) {
            insert_intervals (it->second);
            // std::cout << it->second->liveranges.size() << std::endl;
            std::cout << it->second->liveranges[0].startpoint << " ";
        }
        std::cout << std::endl << "After Sort: " << std::endl;
        for (Interval* inter : all_intervals) {
            std::cout << inter->liveranges[0].startpoint << " ";
        }
        std::cout << std::endl;
        virtual2machine.clear();
        virtual2machine.resize(all_intervals.size(), -1);
    }

    void set_use_contexts(std::map<int, std::vector<int>*> &uc) {
        std::cout << "size_use_contexts:" << uc.size() << std::endl;
        use_contexts.insert(uc.begin(), uc.end());
        for (auto it=use_contexts.begin(); it!=use_contexts.end(); it++) {
            std::cout << it->first << ":" << it->second->size() << std::endl;
        }
    }

private:
    int NUM_REGS;
    /*
    // restore active_set set of live interval in increasing order of endpoint
    std::vector<LiveRange*> active_set;
    // restore live all_intervals of all variables
    std::vector<LiveRange*> all_intervals;
    // restore the free registers
    std::map<int, LiveRange*> register_map;
    */
    // input variables for register allocation
    std::vector<Interval*> all_intervals;
    std::map<int, std::vector<int>*> use_contexts;
    // register allocation process state variables
    std::set<int> active;
    std::set<int> inactive;
    std::set<int> handled;
    std::set<int> unhandled;
    std::map<int, int> register_map; // physical register 2 virtual

    std::vector<int> stack;
    // output variable: gloally restore ssa form mapping
    std::vector<int> virtual2machine;
};

#endif /* end of include guard: REGISTERALLOCATOR_H */
