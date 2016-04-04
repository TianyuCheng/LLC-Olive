#include "RegisterAllocator.h"
#ifndef CLIMITS_H
#include <climits>
#endif

int maxIndexVector (std::vector<int>& vec) {
    int max_elem = -1, max_index = 0;
    for (unsigned i = 0; i < vec.size(); i ++) {
        // std::cout << i << ":" << vec[i] << std::endl;
        if (vec[i] == INT_MAX) {
            return i;
        }
        if (vec[i] > max_elem) {
            max_index = i;
            max_elem = vec[i];
        }
    }
    return max_index;
}

int RegisterAllocator::findNextIntersect(int pos, Interval* cur_itv, Interval* itv) {
    int i = 0, j = 0;
    // move iterator after pos
    int size_cur_itv = cur_itv->liveranges.size();
    int size_itv = itv->liveranges.size();
    for (; i < size_cur_itv; i ++) 
        if (cur_itv->liveranges[i].endpoint >= pos) break;
    for (; j < size_itv; j ++) 
        if (itv->liveranges[j].startpoint >= pos) break;
    int cur_itv_st_idx = i, itv_st_idx = j;
    // non-linear next intersection search
    int visit1_min = INT_MAX, visit2_min = INT_MAX;
    bool found = false;
    for (i = cur_itv_st_idx + 1; !found && i < size_cur_itv; i++) { 
        int startpoint = cur_itv->liveranges[i].startpoint;
        for (j = itv_st_idx; j < size_itv; j ++) {
            if (itv->liveranges[j].startpoint <= startpoint && 
                    startpoint <= itv->liveranges[j].endpoint) {
                visit1_min = startpoint;
                found = true;
                break;
            } else if (startpoint < itv->liveranges[j].startpoint) break;
        }
    }
    found = false;
    for (j = itv_st_idx; !found && j < size_itv; j ++) {
        int startpoint = itv->liveranges[j].startpoint;
        for (i = cur_itv_st_idx; i < size_cur_itv; i ++) {
            if (cur_itv->liveranges[i].startpoint <= startpoint && 
                    startpoint <= cur_itv->liveranges[i].endpoint) {
                visit2_min = startpoint;
                found = true;
                break;
            } else if (startpoint < cur_itv->liveranges[i].startpoint) break;
        }
    }
    return std::min(visit1_min, visit2_min);
}

bool RegisterAllocator::isIntersect(Interval* ia, Interval* ib) {
    unsigned size_ia = ia->liveranges.size();
    unsigned size_ib = ib->liveranges.size();
    for (unsigned i = 0; i < size_ia; i++) { 
        int startpoint = ia->liveranges[i].startpoint;
        int endpoint = ia->liveranges[i].endpoint;
        for (unsigned j = 0; j < size_ib; j ++) {
            if ((ib->liveranges[j].startpoint <= startpoint && startpoint <= ib->liveranges[j].endpoint)
                 || (ib->liveranges[j].startpoint <= endpoint && endpoint <= ib->liveranges[j].endpoint)) {
                return true;
            } else if (endpoint < ib->liveranges[j].startpoint) break;
        }
    }
    for (unsigned j = 0; j < size_ib; j ++) {
        int startpoint = ib->liveranges[j].startpoint;
        int endpoint = ib->liveranges[j].endpoint;
        for (unsigned i = 0; i < size_ia; i++) { 
            if ((ia->liveranges[i].startpoint <= startpoint && startpoint <= ia->liveranges[i].endpoint)
                 || (ia->liveranges[i].startpoint <= endpoint && endpoint <= ia->liveranges[i].endpoint)) {
                return true;
            } else if (endpoint < ia->liveranges[i].startpoint) break;
        }
    }
    return false;
}

int RegisterAllocator::tryAllocateFreeReg(int cur_iid) {
    std::vector<int> freeUntilPos (NUM_REGS, INT_MAX);
   // std::cout << "active_size: " << active.size() << std::endl;
   // std::cout << "inactive_size: " << inactive.size() << std::endl;
    // FOR EACH active interval
    for (int iid : active) {
       //  std::cout << "active_iid: " << iid << std::endl;
        freeUntilPos[virtual2machine[iid]] = 0;
    }
    // FOR EACH inactive interval
    int pos = all_intervals[cur_iid]->liveranges[0].startpoint; // TODO: FIXME
    for (int iid : inactive) {
        Interval* cur_itv = all_intervals[cur_iid]; 
        Interval* itv = all_intervals[iid]; 
        freeUntilPos[iid] = findNextIntersect(pos, cur_itv, itv);
    }
    /*
    for (int x : freeUntilPos) 
        std::cout << x << " "  << std::endl;
    std::cout <<  std::endl;
    */
    return maxIndexVector(freeUntilPos);
}

int RegisterAllocator::findNextUseHelper(std::vector<int>& use_vec, int after) {
        std::cout << "helper start: ";
    for (int use_pos : use_vec) {
        std::cout << use_pos << " ";
        if (use_pos > after) 
            return use_pos;
    }
    std::cout << std::endl;
    return INT_MAX;
}

int RegisterAllocator::findNextUse(int cur_iid, int iid) {
    std::cout << "findNextUse: iid=" << iid << std::endl;
    int start_of_current = all_intervals[cur_iid]->liveranges[0].startpoint;
   //  std::cout << "start_of_current: " << start_of_current << std::endl;
    int next_use;
    if (use_contexts.find(iid) == use_contexts.end())
        next_use = INT_MAX;
    else 
        next_use = findNextUseHelper(*(use_contexts[iid]), start_of_current);
    std::cout << "next_use: " << next_use << std::endl;
    if (next_use < 0)
        assert(false && "No next use! There should be one for the inactive interval");
    return next_use;
}

int RegisterAllocator::allocateBlockedReg(int cur_iid) {
    std::vector<int> nextUsePos (NUM_REGS, INT_MAX);
    // FOR EACH active interval
    for (int iid : active) {
        std::cout << "active_iid: " << iid << std::endl;
        nextUsePos[virtual2machine[iid]] = findNextUse(cur_iid, iid);
    }
    // FOR EACH inactive interval
    for (int iid : inactive) {
        std::cout << "inactive_iid: " << iid << std::endl;
        if (isIntersect(all_intervals[iid], all_intervals[cur_iid])) 
            nextUsePos[iid] = findNextUse(cur_iid, iid);
    }
    return maxIndexVector(nextUsePos);
}

void RegisterAllocator::updateRAState(int cur_iid) {
    // start_of_current
    int pos = all_intervals[cur_iid]->liveranges[0].startpoint;
    // update active
    std::set<int> active2handled; 
    std::set<int> active2inactive;
    for (int iid : active) {
        Interval* iid_it = all_intervals[iid];
        int iid_lr_size = iid_it->liveranges.size();
        int iid_endpoint = iid_it->liveranges.rbegin()->endpoint;
        if (iid_endpoint < pos) 
            active2handled.insert(iid);
        else {
            bool still_active = false;
            for (int i = 0; i < iid_lr_size; i ++) {
                if (iid_it->liveranges[i].startpoint <= pos && 
                     pos <= iid_it->liveranges[i].endpoint)
                    // iid_lr is still active
                    still_active = true;
                    break;
            }
            if (!still_active) active2inactive.insert(iid);
        }
    }
    // update inactive
    std::set<int> inactive2handled; 
    std::set<int> inactive2active;
    for (int iid : inactive) {
        Interval* iid_it = all_intervals[iid];
        int iid_lr_size = iid_it->liveranges.size();
        int iid_endpoint = iid_it->liveranges.rbegin()->endpoint;
        if (iid_endpoint < pos) inactive2handled.insert(iid);
        else {
            bool turn_active = false;
            for (int i = 0; i < iid_lr_size; i ++) {
                if (iid_it->liveranges[i].startpoint <= pos && 
                     pos <= iid_it->liveranges[i].endpoint)
                    // iid_lr is still active
                    turn_active = true;
                    break;
            }
            if (turn_active) inactive2active.insert(iid);
        }
    }   
    // apply all update
    for (int iid : active2inactive) { active.erase(iid); inactive.insert(iid); }
    for (int iid : active2handled) { active.erase(iid); handled.insert(iid); }
    for (int iid : inactive2active) { inactive.erase(iid); active.insert(iid); }
    for (int iid : inactive2handled) { inactive.erase(iid); handled.insert(iid); }
    return ;
}

void RegisterAllocator::resolveConflicts() {
    
}

// split [a,b] to [a, start] and [next_use, b]
void RegisterAllocator::splitInterval(int iid, int start, int reg) {
    Interval* interval = all_intervals[iid];
    // std::cout << "interval():" << iid << std::endl;
    int next_use;
    if (use_contexts.find(iid) == use_contexts.end())
        next_use = INT_MAX;
    else 
        next_use = findNextUseHelper(*(use_contexts[iid]), start);
    // std::cout << "findNextUseHelper():" << std::endl;
    int lr_id = -1;
    for (int i = 0; i < interval->liveranges.size(); i ++) {
        if (interval->liveranges[i].startpoint <= start && 
                start <= interval->liveranges[i].endpoint) {
            lr_id = i; break;
        }
    }
   // std::cout << "find lr_id():" << std::endl;
    if (next_use == INT_MAX) {
        interval->liveranges[lr_id].endpoint = start;
        interval->liveranges[lr_id].pos = reg;
        return ;
    }
    int tmp_endpoint = interval->liveranges[lr_id].endpoint;  // b
    interval->liveranges[lr_id].endpoint = start;
    // insert an new holes
    LiveRange hole (start, next_use);
    stack.push_back(iid);
    hole.set_in_stack (stack.size()-1);
    interval->holes.push_back(hole);

    // insert an new live range
    LiveRange lr (next_use, tmp_endpoint);
    lr.set_in_register(reg);
    // std::cout << "insert():" << std::endl;
    interval->liveranges.insert(interval->liveranges.begin()+lr_id+1, lr);
    return ;
}

/* Linear Scan Algorithm for SSA form (support splitting of interval) */
void RegisterAllocator::linearScanSSA () {
    // initialize four sets recording the system state
    active.clear();
    inactive.clear();
    handled.clear();
    unhandled.clear();
    for (int i = 0; i < iid_start_pairs.size(); i ++) 
        unhandled.insert(iid_start_pairs[i].first);
    // start linear scan algorithm
    std::cout << "Total number of intervals:" << all_intervals.size() << std::endl;
    for (int i = 0; i < iid_start_pairs.size(); i ++) {
        int cur_iid = iid_start_pairs[i].first;
        int cur_start = iid_start_pairs[i].second;

        std::cout << "-------------------------------" << std::endl;
        std::cout << "Interval: " << iid << std::endl;
        std::cout << "updateRAState(): " << std::endl;

        updateRAState(cur_iid); // update Register Allocation State

        std::cout << "active_set_size: "   << active.size() << ", "
                  << "inactive_set_size: " << inactive.size() 
                  << std::endl;
        std::cout << "active_iid: ";
        for (int iid : active) std::cout << iid << " ";
        std::cout <<  std::endl;
        std::cout << "inactive_iid: ";
        for (int iid : inactive) std::cout << iid << " ";
        std::cout <<  std::endl;
        
        int reg;
        if (active.size() == NUM_REGS) {
            // look for one occupied register to allocate
            std::cout << "allocateBlockedReg():" << std::endl;
            reg = allocateBlockedReg(cur_iid);
            int spill = register_map[reg];
            active.erase(spill);
            inactive.insert(spill);
            std::cout << "active_erase: " << spill 
                      << ", active_after: " << active.size()
                      << ", inactive_after: " << inactive.size()
                      << std::endl;
            // TODO: push to stack
            splitInterval(spill, cur_start, reg);
        } else {
            // look for one register to allocate
            std::cout << "tryAllocateFreeReg():"  << std::endl;
            reg = tryAllocateFreeReg(cur_iid); 
        }
        std::cout << "register assigned = " << reg << std::endl;
        register_map[reg] = cur_iid;
        active.insert(cur_iid);
        unhandled.erase(cur_iid);
    }
    // print virtual2machine
    std::cout << "------------virtual2machine---BEGIN-------" << std::endl;
    for (int x : virtual2machine) 
        std::cout << x << std::endl;
    std::cout << "------------virtual2machine----END------" << std::endl;
}

