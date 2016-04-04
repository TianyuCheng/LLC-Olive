#ifndef LIVERANGE_H
#define LIVERANGE_H

#include <string>
#include <vector>
#include <climits>
#include "assert.h"

class LiveRange {
    public:
    int startpoint;
    int endpoint;

    int register_id;
    std::string location;

    LiveRange(int start) {
        startpoint = start;
        endpoint = -1;
    }
    LiveRange(int start, int stop) {
        startpoint = start;
        endpoint = stop;
    }
};

class Interval {
    public:
    std::vector<LiveRange> liveranges; // increasing order of startpoint
    int register_id;  //  
    std::string location;  // location information

    Interval (int start, int stop) {
        LiveRange lr (start, stop);
        liveranges.push_back(lr);
    }
    void addRange(int start, int end) {
        // 1. look for those live ranges where start and end reside
        int start_lr_idx = -1, end_lr_idx = -1;
        int num_live_ranges = liveranges.size();
        for (int i = 0; i < num_live_ranges; i++) 
            if (liveranges[i].startpoint <= start && start < liveranges[i].endpoint) 
                start_lr_idx = i;
        for (int i = 0; i < num_live_ranges; i++) 
            if (liveranges[i].endpoint < end && end <= liveranges[i].endpoint)
                end_lr_idx = i;

        // 2. compute updated startpoint and endpoint
        int updated_startpoint, updated_endpoint;
        if (start_lr_idx < 0) updated_startpoint = start;
        else updated_startpoint = liveranges[start_lr_idx].startpoint;
        if (end_lr_idx < 0) updated_endpoint = end;
        else updated_endpoint = liveranges[end_lr_idx].endpoint;

        // 3. look for all live ranges between [updated_start, updated_end]
        int max_include_idx = INT_MIN, min_include_idx = INT_MAX;
        bool include_found = false; // found liverange included in [start, end]
        for (int i = 0; i < num_live_ranges; i++) 
            if (updated_startpoint <= liveranges[i].startpoint && 
                    liveranges[i].endpoint <= updated_endpoint) {
                if (!include_found) include_found = !include_found;
                max_include_idx = std::max(max_include_idx, i);
                min_include_idx = std::min(min_include_idx, i);
            }

        // 4. modify liveranges
        auto begin_iter = liveranges.begin();
        if (include_found)
            liveranges.erase(begin_iter+min_include_idx, begin_iter+max_include_idx+1);
        begin_iter = liveranges.begin();
        LiveRange new_insert_lr (updated_startpoint, updated_endpoint);
        liveranges.insert(begin_iter+start_lr_idx, new_insert_lr);
    }

    void setFrom(int from, int end_block) {
        LiveRange* lr = &(liveranges[0]);
        std::cout << "lr: " << lr->startpoint << "," << lr->endpoint << std::endl;
        // look for the live range where from resides
        if ( lr->startpoint < from && from < lr->endpoint ) {
            // case 1: shorten point is within the endpoint
            lr->startpoint = from; 
        } /* 
             else if (from == lr->endpoint) {
        // case 2: shorten point is exactly the endpoint (imfromsible)
        assert(false && "shorten point cannot be the endpoint");
        }
        */
    }
};


#endif /* end of include guard: LIVERANGE_H */
