#ifndef REGISTERALLOCATOR_H
#define REGISTERALLOCATOR_H

#include <iostream>
#include <map>
#include <set>
#include <queue>
#include <algorithm>
#include "assert.h"
#include "LiveRange.h"

#define DEBUG 0

class FunctionState;

enum Register {
    RAX, RBX, RCX, RDX,
    RSI, RDI, RBP, RSP,
    R8,  R9,  R10, R11,
    R12, R13, R14, R15
};

// System V X86_64 's calling convention
static Register caller_saved_regs [] = { RAX, RDI, RSI, RDX, RCX, R8, R9, R10, R11 };
static Register callee_saved_regs [] = { RSP, RBP, RBX, R12, R13, R14, R15 };
static Register params_regs [] = { RDI, RSI, RDX, RCX, R8, R9 };

static const char* registers[] = {
    "rax", "rbx", "rcx", "rdx",
    "rsi", "rdi", "rbp", "rsp",
    "r8", "r9", "r10", "r11", 
    "r12", "r13", "r14", "r15", 
};
static int MAX_REGS = 16;
static int REGS_OFFSET = 10;        // we start using registers from REGS_OFFSET

class SimpleRegisterAllocator {
public:
    SimpleRegisterAllocator(int n) : num_regs(n), spillable(0) {
        Reset();
    }
    virtual ~SimpleRegisterAllocator() {
        for (auto it = liveness.begin(); it != liveness.end(); ++it)
            delete it->second;
    }
    void Reset() {
        // num_regs represents number of GENERAL PURPOSE REGISTERS:
        // r10, r11, r12, r13, r14, r15, r16
        for (int i = MAX_REGS-1; i >= std::max(REGS_OFFSET, MAX_REGS-num_regs); i--)
            register_status[i] = -1;      // not used

        // R10 and R11 are disabled during function call arguments preparation
        disabled_regs.insert(R10);
        disabled_regs.insert(R11);

        for (int i = 0; i < virtual2machine.size(); i++)
            virtual2machine[i] = -1;

        spillable = 0;
        nospills.clear();
    }
    void DumpVirtualRegs() {
        std::cerr << "##############################################" << std::endl;
        for (int i = 0; i < virtual2machine.size(); i++)
            std::cerr << "VirtualReg[" << i << "]:\t" << virtual2machine[i] << std::endl;
        std::cerr << "NoSpills?: ";
        for (int i = 0; i < nospills.size(); i++)
            std::cerr << nospills[i] << " ";
        std::cerr << std::endl;
        std::cerr << "##############################################" << std::endl;
    }
    std::string GetMCRegAt(int reg) {
        assert(reg >= 0 && reg < virtual2machine.size());
        if (virtual2machine[reg] < 0 || virtual2machine[reg] >= MAX_REGS) {
            std::cerr << "request virtual reg: " << reg << std::endl;
            DumpVirtualRegs();
        }
        assert(virtual2machine[reg] >= 0 && virtual2machine[reg] < MAX_REGS);
        return std::string(registers[virtual2machine[reg]]);
    }
    int CreateVirtualReg(int reg = -1) {       // -1 for not allocated
        int v = virtual2machine.size();
        virtual2machine.push_back(reg);
        return v;
    }
    int GetRegToSpill(std::ostream &, FunctionState *fstate, int line);
    int GetFreeReg(std::ostream &, FunctionState *fstate, int line);
    void RecordLiveStart(int reg, int startline) {
        auto it = liveness.find(reg);
        if (it == liveness.end()) {
            // register not have a starting point
            liveness[reg] = new LiveRange(startline);
        }
        else {
            RecordLiveStop(reg, startline);
        }
    }
    void RecordLiveStop(int reg, int endline) {
        auto it = liveness.find(reg);
        if (it == liveness.end()) {
            std::cerr << "Requested Virtual Reg: " << reg << std::endl;
            DumpVirtualRegs();
        }
        assert(it != liveness.end());
        liveness[reg]->AddInnerPoint(endline);
    }
    void PrintLiveness(std::ostream &out) {
        out << "####################################################" << std::endl;
        for (auto it = liveness.begin(); it != liveness.end(); ++it) {
            LiveRange *range = it->second;
            out << "# Virtual Register " << it->first << ":\t" 
                << "start:\t" << range->startpoint << "\t"
                << "end:\t"   << range->endpoint << "\t"
                << "inner points: ";
            for (auto iter = it->second->innerpoints.begin();
                    iter != it->second->innerpoints.end(); ++iter)
                out << *iter << " ";
            out << std::endl;
        }
        out << "####################################################" << std::endl;
    }
    void Allocate(FunctionState *fstate, std::ostream &out, int reg, int line);
    bool RegisterInUse(Register r) {
        auto it = register_status.find(r);
        if (it == register_status.end()) return false;
        return it->second >= 0;
    }

    void ResetNoSpills() { 
#if DEBUG
    std::cerr << "=========== RESET NO-SPILLS ===========" << std::endl;
#endif
        nospills.clear();
        nospills.resize(0);
    }
    bool CanSpill(int vir_reg) {
        auto it = std::find(nospills.begin(), nospills.end(), vir_reg);
        return it == nospills.end();
    }

    void EnableSpill(FunctionState *fstate, std::ostream &out);
    void DisableSpill(FunctionState *fstate, std::ostream &out);

private:
    int num_regs;
    int spillable;
    std::map<int, LiveRange*> liveness;
    std::map<int, int> register_status;
    std::vector<int> virtual2machine;   // map to value > 0 (good registers), value < 0 (memory), value = -1 (not allocated)
    std::vector<int> nospills;          // registers used in current instruction, cannot spill it
    std::set<Register> disabled_regs;
};


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
        all_intervals.insert(ai.begin(), ai.end());
        for (auto it=ai.rbegin(); it!=ai.rend(); it++) {
            for (int i = 0; i < it->second->liveranges.size(); i ++) {
                int start = it->second->liveranges[i].startpoint;
                std::cout << it->second->liveranges[i].startpoint << " ";
                insert_intervals (it->first, start);
            }
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
    std::vector<int> stack;

    std::vector<int> assigned_registers;
    std::vector<std::map<int, int>> transitions;

    // output variable:  restore mapping from virtual to physical
    std::vector<int> virtual2machine;
};

#endif /* end of include guard: REGISTERALLOCATOR_H */
