#include "RegisterAllocator.h"
#ifndef CLIMITS_H
#include <climits>
#endif

int maxIndexVector (vector<int>& vec) {
    int max_elem = -1, max_index = 0;
    for (int i = 0; i < vec.size(); i ++) {
        if (vec[i] == INT_MAX) return i;
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
    int size_ia = ia->liveranges.size();
    int size_ib = ib->liveranges.size();
    for (i = 0; i < size_ia; i++) { 
        int startpoint = ia->liveranges[i].startpoint;
        int endpoint = ia->liveranges[i].endpoint;
        for (j = 0; j < size_ib; j ++) {
            if ((ib->liveranges[j].startpoint <= startpoint && startpoint <= ib->liveranges[j].endpoint)
                 || (ib->liveranges[j].startpoint <= endpoint && endpoint <= ib->liveranges[j].endpoint)) {
                return true;
            } else if (endpoint < ib->liveranges[j].startpoint) break;
        }
    }
    for (j = 0; j < size_ib; j ++) {
        int startpoint = ib->liveranges[j].startpoint;
        int endpoint = ib->liveranges[j].endpoint;
        for (i = 0; i < size_ia; i++) { 
            if ((ia->liveranges[i].startpoint <= startpoint && startpoint <= ia->liveranges[i].endpoint)
                 || (ia->liveranges[i].startpoint <= endpoint && endpoint <= ia->liveranges[i].endpoint)) {
                return true;
            } else if (endpoint < ia->liveranges[i].startpoint) break;
        }
    }
    return false;
}

int RegisterAllocator::tryAllocateFreeReg(int cur_iid) {
    vector<int> freeUntilPos (NUM_REGS, INT_MAX);
    // FOR EACH active interval
    for (int iid : active) freeUntilPos[iid] = 0;
    // FOR EACH inactive interval
    int pos = all_intervals[cur_iid].liveranges[0].startpoint; // TODO: FIXME
    for (int iid : inactive) {
        Interval* cur_itv = all_intervals[cur_iid]; 
        Interval* itv = all_intervals[iid]; 
        freeUntilPos[iid] = findNextIntersect(pos, cur_itv, itv);
    }
    return maxIndexVector(freeUntilPos);
}

int RegisterAllocator::findNextUse(int cur_iid, int iid) {
    int start_of_current = all_intervals[cur_iid].liveranges[0].startpoint;
    int next_use = findNextUseHelper(use_contexts[iid], start_of_current);
    if (next_use < 0)
        assert(false && "No next use! There should be one for the inactive interval");
    return next_use;
}

int RegisterAllocator::findNextUse(std::vector<int> use_vec, int after) {
    for (int use_pos : use_vec) 
        if (use_pos > after) 
            return use_pos;
    return -1;
}

int RegisterAllocator::allocateBlockedReg(int cur_iid) {
    vector<int> nextUsePos (NUM_REGS, INT_MAX);
    // FOR EACH active interval
    for (int iid : active) 
        nextUsePos[iid] = findNextUse(cur_iid, iid);
    // FOR EACH inactive interval
    for (int iid : inactive) 
        if (isIntersect(all_intervals[iid], all_intervals[cur_iid])) 
            nextUsePos[iid] = findNextUse(cur_iid, iid);
    return maxIndexVector(nextUsePos);
}

void RegisterAllocator::expirePrevIntervals(int cur_iid) {
    for () {
        if () {

        }
    }
    return ;
}

void RegisterAllocator::resolve() {

}

/* Linear Scan Algorithm for SSA form (support splitting of interval) */
void RegisterAllocator::linearScanSSA () {
    // initialize four sets recording the system state
    active.clear();
    inactive.clear();
    handled.clear();
    unhandled.clear();
    for (int i = 0; i < all_intervals.size(); i ++) unhandled.insert(i);
    // start linear scan algorithm
    for (int i = 0; i < all_intervals.size(); i ++) {
        expirePrevIntervals(i);
        if (active_set.size() == NUM_REGS) {
            // look for one occupied register to allocate
            allocateBlockedReg(i);
            //  system state transition
            
        } else {
            // look for one register to allocate
            tryAllocateFreeReg(i); 
            // system state transition
            active.insert(i);
        }
        unhandled.erase(i);
    }
}

/*
void RegisterAllocator::linearScanNaive () {
    this->active_set.clear();
    for (int i = 0; i < all_intervals.size(); i ++) {
        expireOldIntervals(i);
        if (active_set.size() == NUM_REGS) {
            spillAtInterval(i);
        } else {
            // assign a free register to the interval
            for (auto it=register_map.begin(); it!=register_map.end(); ++it) {
                if (it->second == NULL) {
                    it->second = all_intervals[i];
                    all_intervals[i]->register_id = it->first;
                    virtual2machine[i] = it->first;
                    break;
                }
            }
            // put the interval to active set
            insert_active_set (active_set, all_intervals[i]);
        }
    }
}

void RegisterAllocator::expireOldIntervals (int i) {
    // in order of increasing end point
    int size_active_set = active_set.size();
    int remove_point = 0;
    for (int j = 0; j < size_active_set; j ++) {
        if (active_set[j]->endpoint >= all_intervals[i]->startpoint) {
            remove_point = j;
            break;
        }
    }
    // add their register to free register pool
    for (int j = 0; j < remove_point; j ++) {
        int old_reg_id = active_set[j]->register_id;
        if (old_reg_id < 0) std::cerr << "old_reg_id must >= 0" << std::endl;
        register_map[old_reg_id] = NULL;
        active_set[j]->register_id = -1;
    }
    // remove expired active_set interval
    active_set.erase(active_set.begin(), active_set.begin()+remove_point);
}

void RegisterAllocator::spillAtInterval (int i) {
    LiveRange* spill = active_set[active_set.size()-1];
    if (spill->endpoint > all_intervals[i]->endpoint) {
        // spill the last interval of active_set set
        if (spill->register_id < 0) std::cerr << "spill reg id must >= 0" << std::endl;
        all_intervals[i]->register_id = spill->register_id;
        virtual2machine[i] = spill->register_id;
        spill->location = "-1"; // TODO: TEMPORARILY PUT -1 for compilation
        // active_set.erase(spill);
        active_set.erase(active_set.end() - 1);     // this should be equivalent to the previous line
        insert_active_set (active_set, all_intervals[i]);
    } else {
        // spill i directly
        // locations[i] = -1;   // TODO: TEMPORARILY PUT -1 for compilation
    }
}
*/
