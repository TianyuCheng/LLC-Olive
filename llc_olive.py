#!/usr/bin/env python2

with open("./llc_olive_helper.brg", "w") as f:
    grammar = '''
%term BURP=0 ADDI=309 ADDRLP=295
%declare<void> stmt<int indent>;
%%
stmt: BURP
    { return 1; }
    = {
        std::cout<< "Hello World!" << std::endl;
    };
%%
    '''
    print >> f, grammar

with open("./llc_olive.brg", "w") as f:
    include = file("./llc_olive_helper.h", "r")
    grammar = file("./llc_olive_helper.brg", "r")
    implemt = file("./llc_olive_helper.cpp", "r")

    # combine and write out
    print >> f, """%%{
%s
%%}

%s

%s """ % (include.read(), grammar.read(), implemt.read())

    # close file handles
    include.close()
    grammar.close()
    implemt.close()
