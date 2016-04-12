llc-olive
=============

Authors
-------

- Student I
    + Name: Tianyu Cheng
    + EID: tc26752
    + Email: tianyu.cheng@utexas.edu

- Student II
    + Name: Xin Lin
    + EID: xl5224
    + Email: jimmylin@utexas.edu

Usage
-----

To compile our code, type

```
    make
```

To run tests, type

```
    make clean test
```

Our build system supports different parameters for tests, type

```
    # MEM1REG=1 tests our compiler without phi functions
    # MEM2REG=1 tests our compiler with phi functions
    # NUM_REGS  represents number of registers used in the compiler
    #           in our assumption, we only use r10, r11, r12, r13, r14, r15
    #           as general purpose register, therefore the maximum number
    #           of registers is 6
    make clean test MEM2REG=0 NUM_REGS=3
```

References
--------
We referred to the following llvm built-in libraries and theirofficial online documentations:

Acknowledgements
-------
We would like to give many thanks to Prof. Pingali and our Teaching Assistant Mr. Roshan for their teaching and instructions. 
