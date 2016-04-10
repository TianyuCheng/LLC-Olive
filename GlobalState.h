#ifndef GLOBALSTATE_H
#define GLOBALSTATE_H

#include <string>
#include <fstream>
#include <iostream>

#include <map>
#include <set>
#include <vector>
#include <cassert>

#include "Value.h"
#include "Tree.h"

class GlobalState
{
public:
    GlobalState() {}
    virtual ~GlobalState() {}

    void AddGlobalVariable(llvm::GlobalValue *global, Tree *t) {
        globals.insert(std::pair<llvm::GlobalValue*, Tree*>(global, t));
    }

    Tree* FindFromGlobalMap(llvm::GlobalValue *global) const {
        auto it = globals.find(global);
        if (it != globals.end())
            return it->second;
        return nullptr;
    }

private:
    /* data */
    std::map<llvm::GlobalValue*, Tree*> globals;
};

#endif /* end of include guard: GLOBALSTATE_H */
