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

Questions
---------

- Instruction Selection
    - Expression Tree Construction
        - How can we use dataflow graph in the SSA form to build expression trees?
        - What should be the root of the expression tree?
        - Can two expression trees have overlapping nodes (Overlapping nodes would be recomputed)?
        - How do these choices affect register allocation?
    - Execution Time Reduction
        - Options to reduce by
            - generating fewer instructions
            - generating instructions with fewer memory accesses
            - generating simpler instructions, etc
        - How to assign cost?
            - At current we do not know, so mostly by experimentation (using tools like OpenTuner)
            - Possible to apply machine learning algorithm to pick values

- Register Allocation
    - linear scan
    - graph coloring

References
--------
We referred to the following llvm built-in libraries and theirofficial online documentations:

Acknowledgements
-------
We would like to give many thanks to Prof. Pingali and our Teaching Assistant Mr. Roshan for their teaching and instructions. 
