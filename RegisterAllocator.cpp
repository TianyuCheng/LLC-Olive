#include "RegisterAllocator.h"

void RegisterAllocator::buildIntervals() {
    
}

void RegisterAllocator::allocateFreeReg() {

}

void RegisterAllocator::allocateBlockedReg() {

}

void RegisterAllocator::linearScanAllocate () {
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
