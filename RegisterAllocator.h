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
    }

    virtual ~RegisterAllocator() { }

    std::string Allocate() {
        // TODO: interaction 
        // return std::string("rax");
    }

    bool isIntersect (Interval* ia, Interval* ib);
    int findNextIntersect (int pos, Interval* cur_itv, Interval* itv);
    int findNextUseHelper(std::vector<int>& use_vec, int after);
    int findNextUse (int cur_iid, int iid, int cur_start);
    void splitInterval(int iid, int start, int reg);

    void updateRAState(int cur_iid);
    int tryAllocateFreeReg (int cur_iid);
    int allocateBlockedReg (int cur_iid);
    void resolveConflicts ();
    void linearScanSSA ();

    std::vector<int> get_virtual2machine() const {
        return virtual2machine;
    }

    void insert_intervals (int iid, int start) {
        iid_start_pairs.push_back(std::make_pair(iid, start));
        for (int i = iid_start_pairs.size()-1; i > 0 ; i --) {
            int cur_startpoint = iid_start_pairs[i].second;
            int prev_startpoint = iid_start_pairs[i-1].second;
            if ( prev_startpoint <= cur_startpoint ) break;
            else {
                int tmp_iid = iid_start_pairs[i].first;
                int tmp_start = iid_start_pairs[i].second;
                iid_start_pairs[i].first = iid_start_pairs[i-1].first;
                iid_start_pairs[i].second = iid_start_pairs[i-1].second;
                iid_start_pairs[i-1].first = tmp_iid;
                iid_start_pairs[i-1].second = tmp_start;
            }
        }
    }
    void set_all_intervals(std::map<int, Interval*> &ai) {
        std::cout << "Startpoint of Built intervals" << std::endl;
        for (auto it=ai.rbegin(); it!=ai.rend(); it++) {
            int start = it->second->liveranges[0].startpoint;
            std::cout << it->second->liveranges[0].startpoint << " ";
            all_intervals.insert(ai.begin(), ai.end());
            insert_intervals (it->first, start);
            // std::cout << it->second->liveranges.size() << std::endl;
        }
        std::cout << std::endl << "After Sort: " << std::endl;
        for (int i = 0; i < iid_start_pairs.size(); i ++) {
            std::cout << iid_start_pairs[i].second << " ";
        }
        std::cout << std::endl;
        virtual2machine.clear();
        virtual2machine.resize(iid_start_pairs.size(), -1);
    }

    void set_use_contexts(std::map<int, std::vector<int>*> &uc) {
        std::cout << "size_use_contexts:" << uc.size() << std::endl;
        use_contexts.insert(uc.begin(), uc.end());
    }

private:
    int NUM_REGS;
    // input variables for register allocation
    // std::vector<Interval*> all_intervals;
    std::map<int, Interval*> all_intervals;
    std::map<int, std::vector<int>*> use_contexts;
    // register allocation process state variables
    std::set<int> active;
    std::set<int> inactive;
    std::set<int> handled;
    std::set<int> unhandled;
    std::vector<std::pair<int, int>> iid_start_pairs; // <iid, start>s for splitted intervals
    std::map<int, int> register_map; // physical register 2 interval id (virtual) 

    std::vector<std::map<int, int>> transitions;

    std::vector<int> stack;
    // output variable:  restore mapping from virtual to physical
    std::vector<int> virtual2machine;
};

#endif /* end of include guard: REGISTERALLOCATOR_H */
