#ifndef REGISTERALLOCATOR_H
#define REGISTERALLOCATOR_H

#include <iostream>

static const char* registers[] = {
    "rax", "rbx", "rcx", "rdx",
    "rdi", "rsi", "rbp", "rsp",
    "r8", "r9", "r10", "r11", 
    "r12d", "r13", "r14", "r15"
};
static int NUM_REGS = 16;

/**
 * Register Allocator
 * */
class RegisterAllocator
{
public:
    RegisterAllocator(int num_regs) {
    }
    virtual ~RegisterAllocator() {
    }
    std::string Allocate() {
        return std::string("rax");
    }

private:
    int num_regs;
};

#endif /* end of include guard: REGISTERALLOCATOR_H */
