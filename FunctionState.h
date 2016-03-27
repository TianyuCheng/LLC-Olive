#ifndef FUNCTIONSTATE_H
#define FUNCTIONSTATE_H

#include <string>
#include <fstream>
#include <iostream>
#include <vector>

#include "RegisterAllocator.h"

class X86Inst
{
public:
    X86Inst(const char *op, const char *dest, const char *src);
    virtual ~X86Inst();

private:
};

/**
 * Function State Keeper
 * */
class FunctionState
{
public:
    FunctionState(std::string name, int num_regs, int loop_label = 0);
    virtual ~FunctionState();

    int GetVirtualReg() {
        int v = virtual2machine.size();
        virtual2machine.push_back(-2);      // -2 not used yet
                                            // -1 not allocated yet
        return v;
    }
    void PrintAssembly(std::ostream &out);

private:
    void LiveRangeAnalysis();
    std::string GetMachineReg(int virtual_reg);

private:
    // information about the function
    std::string function_name;
    int loop_label;
    // do we want to keep track of number of locals

    // information about instruction and registers
    std::vector<int> virtual2machine;
    std::vector<X86Inst> assembly;
    RegisterAllocator allocator;
};

typedef FunctionState* FUNCTION_STATE;

#endif /* end of include guard: FUNCTIONSTATE_H */
