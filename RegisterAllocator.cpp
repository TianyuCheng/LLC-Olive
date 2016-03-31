#include "RegisterAllocator.h"

void RegisterAllocator::linearScanAllocate () {
    this.active_set.clear();
    for (int i = 0; i < all_intervals.size(); i ++) {
        expireOldIntervals(i);
        if (active_set.size() == NUM_REGS) {
            spillAtInterval(i);
        } else {
            // assign a free register to the interval
            for (auto it=register_map.begin(); it!=register_map.end(); ++it) {
                if (it.second == NULL) {
                    it.second = all_intervals[i];
                    all_intervals[i]->register_id = it.first;
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
        register_map[old_reg_id] = NULL;
        active_set[j]->register_id = -1;
    }
    // remove expired active_set interval
    if (remove_point > 0) 
        active_set.erase(active_set.begin(), active_set.begin()+remove_point);
}

void RegisterAllocator::spillAtInterval (int i) {
    LiveRange* spill = active_set[active_set.size()-1];
    if (spill.endpoint > all_intervals[i]->endpoint) {
        // spill the last interval of active_set set
        all_intervals[i]->register_id = spill->register_id;
        spill.locations = ; // TODO; 
        active_set.erase(spill);
        insert_active_set (active_set, all_intervals[i]);
    } else {
        // spill i directly
        locations[i] = ;   // TODO:
    }
}
