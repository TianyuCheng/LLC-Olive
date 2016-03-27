#include "FunctionState.h"

FunctionState::FunctionState(std::string name, int n, int l)
    : function_name(name), loop_label(l), num_locals(0), allocator(n)
{
    // initialize function state here
}

FunctionState::~FunctionState() {
    // de-initialize function state here
}

std::string FunctionState::GetMachineReg(int virtual_reg) {
    // allocate a free register
    return allocator.Allocate();
}

void FunctionState::PrintAssembly(std::ostream &out) {
    // print assembly to file
    // TODO: remember to print function begin and ends
}

void FunctionState::LiveRangeAnalysis() {
    // analyze live range of virtual registers
}
