#ifndef LIVERANGE_H
#define LIVERANGE_H

#include <string>

struct LiveRange {
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


#endif /* end of include guard: LIVERANGE_H */
