#!/usr/bin/env python2

term_ops = {
    ( 1, "Ret           ")
   ,( 2, "Br            ")
   ,( 3, "Switch        ")
   ,( 4, "IndirectBr    ")
   ,( 5, "Invoke        ")
   ,( 6, "Resume        ")
   ,( 7, "Unreachable   ")
   ,( 8, "CleanupRet    ")
   ,( 9, "CatchRet      ")
   ,(10, "CatchSwitch   ")
}

binary_ops = {
    (11, "Add  ")
   ,(12, "FAdd ")
   ,(13, "Sub  ")
   ,(14, "FSub ")
   ,(15, "Mul  ")
   ,(16, "FMul ")
   ,(17, "UDiv ")
   ,(18, "SDiv ")
   ,(19, "FDiv ")
   ,(20, "URem ")
   ,(21, "SRem ")
   ,(22, "FRem ")
}

logical_ops = {
    (23, "Shl ")
   ,(24, "LShr")
   ,(25, "AShr")
   ,(26, "And ")
   ,(27, "Or  ")
   ,(28, "Xor ")
}

memory_ops = {
    (29, "Alloca        ")
   ,(30, "Load          ")
   ,(31, "Store         ")
   ,(32, "GetElementPtr ")
   ,(33, "Fence         ")
   ,(34, "AtomicCmpXchg ")
   ,(35, "AtomicRMW     ")
}

cast_ops = {
    (36, "Trunc   ")
   ,(37, "ZExt    ")
   ,(38, "SExt    ")
   ,(39, "FPToUI  ")
   ,(40, "FPToSI  ")
   ,(41, "UIToFP  ")
   ,(42, "SIToFP  ")
   ,(43, "FPTrunc ")
   ,(44, "FPExt   ")
   ,(45, "PtrToInt")
   ,(46, "IntToPtr")
   ,(47, "BitCast ")
   ,(48, "AddrSpaceCast")
}

# we probably do not need this
first_funclepad_ops = {
    (49, "CleanupPad")
   ,(50, "CatchPad  ")
}

other_ops = {
    (51, "ICmp   ")
   ,(52, "FCmp   ")
   ,(53, "PHI    ")
   ,(54, "Call   ")
   ,(55, "Select ")
   ,(56, "UserOp1")
   ,(57, "UserOp2")
   ,(58, "VAArg  ")
   ,(59, "ExtractElement")
   ,(60, "InsertElement")
   ,(61, "ShuffleVector")
   ,(62, "ExtractValue")
   ,(63, "InsertValue")
   ,(64, "LandingPad")
}

# clean up spacing
term_ops = map(lambda (x, y): (x, y.strip()), term_ops)
binary_ops = map(lambda (x, y): (x, y.strip()), binary_ops)
logical_ops = map(lambda (x, y): (x, y.strip()), logical_ops)
memory_ops = map(lambda (x, y): (x, y.strip()), memory_ops)
cast_ops = map(lambda (x, y): (x, y.strip()), cast_ops)
first_funclepad_ops = map(lambda (x, y): (x, y.strip()), first_funclepad_ops)
other_ops = map(lambda (x, y): (x, y.strip()), other_ops)

def gen_terms(ops):
    return  '%term ' + ' '.join(map(lambda (num, name): "%s=%d" % (name, num), ops))

def gen_nonterms(ops):
    return '\n'.join(map(lambda (_, name): "%%declare<void> %s<int indent>;" % name.lower(), ops))

def gen_binary_rules(ops):
    ret = []
    for _, op in ops:
        s1 = """stmt:\t%s { return 1; } = { };""" % (op.lower())
        s2 = """%s:\t%s(ri, rim) { return 1; } = { };""" % (op.lower(), op)
        ret.append(s1)
        ret.append(s2)
        ret.append("")
    return '\n'.join(ret)

#   ____                                           
#  / ___|_ __ __ _ _ __ ___  _ __ ___   __ _ _ __  
# | |  _| '__/ _` | '_ ` _ \| '_ ` _ \ / _` | '__| 
# | |_| | | | (_| | | | | | | | | | | | (_| | |    
#  \____|_|  \__,_|_| |_| |_|_| |_| |_|\__,_|_|    
#                                                  
with open("./llc_olive_helper.brg", "w") as f:
    # generate all terminals here
    print >> f, "%term REG IMM MEM"
    # print every instruction needed here
    print >> f, gen_terms(term_ops)
    print >> f, gen_terms(binary_ops)
    print >> f, gen_terms(logical_ops)
    print >> f, gen_terms(memory_ops)
    print >> f, gen_terms(cast_ops)
    print >> f, gen_terms(first_funclepad_ops)
    print >> f, gen_terms(other_ops)

    # generate all non-terminals here
    print >> f, '''
%declare<void> stmt<int indent>;
%declare<void> rim<int indent>;
%declare<void> ri<int indent>;
%declare<void> reg<int indent>;
%declare<void> imm<int indent>;
%declare<void> mem<int indent>;
    '''
    # print every instruction needed here
    # print >> f, gen_nonterms(term_ops)
    print >> f, gen_nonterms(binary_ops)
    # print >> f, gen_nonterms(logical_ops)
    # print >> f, gen_nonterms(memory_ops)
    # print >> f, gen_nonterms(cast_ops)
    # print >> f, gen_nonterms(first_funclepad_ops)
    # print >> f, gen_nonterms(other_ops)
    print >> f, "%%"

    # generate all rules here
    print >> f, '''
rim:   reg    { return 1; } = { /* TODO: match reg here */ };
rim:   imm    { return 1; } = { /* TODO: match imm here */ };
rim:   mem    { return 1; } = { /* TODO: match mem here */ };
ri:    reg    { return 1; } = { /* TODO: match reg here */ };
ri:    imm    { return 1; } = { /* TODO: match imm here */ };
reg:   REG(_) { return 1; } = { /* TODO: match reg here */ };
imm:   IMM(_) { return 1; } = { /* TODO: match imm here */ };
mem:   MEM(_) { return 1; } = { /* TODO: match mem here */ };
    '''
    print >> f, gen_binary_rules(binary_ops)

#     # print every instruction rule needed here
#     print >> f, '''
# stmt:  add { return 1; } = { };
#
# add:   ADD(ri, rim) 
#     { return 1; }
#     =
#     {
#         std::cerr << "matched ADD(ri, mri)" << std::endl;
#     };
# %%
#     '''
    print >> f, "%%"

#   ____                           _              
#  / ___| ___ _ __   ___ _ __ __ _| |_ ___  _ __  
# | |  _ / _ \ '_ \ / _ \ '__/ _` | __/ _ \| '__| 
# | |_| |  __/ | | |  __/ | | (_| | || (_) | |    
#  \____|\___|_| |_|\___|_|  \__,_|\__\___/|_|    
#                                                 
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
