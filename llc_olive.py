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
term_ops            = sorted(map(lambda (x, y): (x, y.strip()), term_ops), key=lambda (x, _): x)
binary_ops          = sorted(map(lambda (x, y): (x, y.strip()), binary_ops), key=lambda (x, _): x)
logical_ops         = sorted(map(lambda (x, y): (x, y.strip()), logical_ops), key=lambda (x, _): x)
memory_ops          = sorted(map(lambda (x, y): (x, y.strip()), memory_ops), key=lambda (x, _): x)
cast_ops            = sorted(map(lambda (x, y): (x, y.strip()), cast_ops), key=lambda (x, _): x)
first_funclepad_ops = sorted(map(lambda (x, y): (x, y.strip()), first_funclepad_ops), key=lambda (x, _): x)
other_ops           = sorted(map(lambda (x, y): (x, y.strip()), other_ops), key=lambda (x, _): x)

def gen_enums(ops):
    return "enum { " + ', '.join(map(lambda (num, name): "%s=%d" % (name, num), ops)) + " };"

# def gen_terms(ops):
#     return  '%term ' + ' '.join(map(lambda (num, name): "%s=%d" % (name, num), ops))
#
# def gen_nonterms(ops):
#     return '\n'.join(map(lambda (_, name): "%%declare<void> %s<FUNCTION_STATE fstate>;" % name.lower(), ops))
#
# def gen_binary_rules(ops):
#     op_costs = {
#         "Add": 1, "FAdd": 5, "Sub": 1, "FSub": 5, "Mul": 2, "FMul": 10,
#         "UDiv": 4, "SDiv": 4, "FDiv": 20, "URem": 6, "SRem": 6, "FRem": 30
#     }
#     ret = []
#     for _, op in ops:
#         s1 = """stmt:\t%s { $cost[0].cost = $cost[1].cost; } = { };""" % (op.lower())
#         s2 = """%s:\t%s(rim, ri) { $cost[0].cost = $cost[2].cost + $cost[3].cost + %d; } = {
#             if (OP_LABEL($2) == IMM)
#                 std::cerr << "OPERAND IS IMM: " << $2->val.val.i32s << std::endl;
#             if (OP_LABEL($3) == IMM)
#                 std::cerr << "OPERAND IS IMM: " << $3->val.val.i32s << std::endl;
#         };""" % (op.lower(), op, op_costs[op])
#         ret.append(s1)
#         ret.append(s2)
#         ret.append("")
#     return '\n'.join(ret)
#
# def gen_logical_rules(ops):
#     ret = []
#     for _, op in ops:
#         s1 = """stmt:\t%s { $cost[0].cost = $cost[1].cost; } = { };""" % (op.lower())
#         s2 = """%s:\t%s(rim, ri) { $cost[0].cost = $cost[2].cost + $cost[3].cost + 1; } = { };""" % (op.lower(), op)
#         ret.append(s1)
#         ret.append(s2)
#         ret.append("")
#     return '\n'.join(ret)
#
# def gen_memory_rules(ops):
#     ret = []
#     for _, op in ops:
#         s1 = """stmt:\t%s { $cost[0].cost = $cost[1].cost; } = { 
#             std::cerr << "Hello, Stmt -> %s" << std::endl; 
#             $action[1](fstate);
#             };""" % (op.lower(), op)
#         if op == "Load":
#             s2 = """%s:\t%s(ri) { $cost[0].cost = $cost[2].cost + 1; } = { 
#                 std::cerr << "Hello Load" << std::endl;
#                 $action[2](fstate);
#             };""" % (op.lower(), op)
#         elif op == "Alloca":
#             s2 = """%s:\t%s(ri) { $cost[0].cost = $cost[2].cost + 1; } = {
#                 std::cerr << "Hello Alloca" << std::endl;
#                 $action[2](fstate);
#             };""" % (op.lower(), op)
#         elif op == "Store":
#             s2 = """%s:\t%s(rim, ri) { $cost[0].cost = $cost[2].cost + $cost[3].cost + 1; } = {
#                 std::cerr << "Hello Store" << std::endl;
#                 $action[2](fstate);
#                 $action[3](fstate);
#             };""" % (op.lower(), op)
#         else:
#             s2 = """%s:\t%s(rim, ri) { $cost[0].cost = $cost[2].cost + $cost[3].cost + 1; } = { };""" % (op.lower(), op)
#         ret.append(s1)
#         ret.append(s2)
#         ret.append("")
#     return '\n'.join(ret)
#
# def gen_term_rules(ops):
#     ret = []
#     for _, op in ops:
#         s1 = """stmt:\t%s { $cost[0].cost = $cost[1].cost; } = { };""" % (op.lower())
#         if op == "Ret":
#             s2 = """%s:\t%s(ri) { $cost[0].cost = $cost[2].cost + 1; } = { };""" % (op.lower(), op)
#         else:
#             s2 = """%s:\t%s(ri) { $cost[0].cost = $cost[2].cost + 1; } = { };""" % (op.lower(), op)
#         ret.append(s1)
#         ret.append(s2)
#         ret.append("")
#     return '\n'.join(ret)
#
# #   ____                                           
# #  / ___|_ __ __ _ _ __ ___  _ __ ___   __ _ _ __  
# # | |  _| '__/ _` | '_ ` _ \| '_ ` _ \ / _` | '__| 
# # | |_| | | | (_| | | | | | | | | | | | (_| | |    
# #  \____|_|  \__,_|_| |_| |_|_| |_| |_|\__,_|_|    
# #                                                  
# # generate all terminals here
# print gen_terms(term_ops)
# print gen_terms(binary_ops)
# print gen_terms(logical_ops)
# print gen_terms(memory_ops)
# print gen_terms(cast_ops)
# print gen_terms(first_funclepad_ops)
# print gen_terms(other_ops)
# print "%term REG IMM MEM"
#
# # generate all non-terminals here
# print '''
# %declare<void> stmt<FUNCTION_STATE fstate>;
# %declare<void> rim<FUNCTION_STATE  fstate>;
# %declare<void> ri<FUNCTION_STATE   fstate>;
# %declare<void> reg<FUNCTION_STATE  fstate>;
# %declare<void> imm<FUNCTION_STATE  fstate>;
# %declare<void> mem<FUNCTION_STATE  fstate>;
# '''
# # print every instruction needed here
# print gen_nonterms(term_ops)
# print gen_nonterms(binary_ops)
# print gen_nonterms(logical_ops)
# print gen_nonterms(memory_ops)
# # print gen_nonterms(cast_ops)
# # print gen_nonterms(first_funclepad_ops)
# # print gen_nonterms(other_ops)
# print "%%"
#
# # generate all rules here
# print '''
# ri:    stmt   { $cost[0].cost = $cost[1].cost; } = { $action[1](fstate); };
# rim:   stmt   { $cost[0].cost = $cost[1].cost; } = { $action[1](fstate); };
# rim:   reg    { $cost[0].cost = $cost[1].cost; } = { $action[1](fstate); };
# rim:   imm    { $cost[0].cost = $cost[1].cost; } = { $action[1](fstate); };
# rim:   mem    { $cost[0].cost = $cost[1].cost; } = { $action[1](fstate); };
# ri:    reg    { $cost[0].cost = $cost[1].cost; } = { $action[1](fstate); };
# ri:    imm    { $cost[0].cost = $cost[1].cost; } = { $action[1](fstate); };
# reg:   REG    { $cost[0].cost = 1;             } = { std::cerr << "Hello REG " << $0->val.val.i32s << std::endl; };
# imm:   IMM    { $cost[0].cost = 0;             } = { std::cerr << "Hello IMM " << $0->val.val.i32s << std::endl; };
# mem:   MEM    { $cost[0].cost = 10;            } = { std::cerr << "Hello MEM " << $0->val.val.i32s << std::endl; };
# '''
# print gen_term_rules(term_ops)
# print gen_binary_rules(binary_ops)
# print gen_memory_rules(memory_ops)
# print gen_logical_rules(logical_ops)
# print "%%"


#  _____                            
# | ____|_ __  _   _ _ __ ___  ___  
# |  _| | '_ \| | | | '_ ` _ \/ __| 
# | |___| | | | |_| | | | | | \__ \ 
# |_____|_| |_|\__,_|_| |_| |_|___/ 
#                                   
with open("./llc_olive_helper.enums", "w") as f:
    print >> f, gen_enums(term_ops)
    print >> f, gen_enums(binary_ops)
    print >> f, gen_enums(logical_ops)
    print >> f, gen_enums(memory_ops)
    print >> f, gen_enums(cast_ops)
    print >> f, gen_enums(first_funclepad_ops)
    print >> f, gen_enums(other_ops)
    print >> f, "enum { REG=65, IMM=66, MEM=67, LABEL=68, DUMMY=0 };"

#   ____                           _              
#  / ___| ___ _ __   ___ _ __ __ _| |_ ___  _ __  
# | |  _ / _ \ '_ \ / _ \ '__/ _` | __/ _ \| '__| 
# | |_| |  __/ | | |  __/ | | (_| | || (_) | |    
#  \____|\___|_| |_|\___|_|  \__,_|\__\___/|_|    
#                                                 
with open("./llc_olive.brg", "w") as f:
    include = file("./llc_olive_helper.hhh", "r")
    enums   = file("./llc_olive_helper.enums", "r")
    grammar = file("./llc_olive_helper.brg", "r")
    implemt = file("./llc_olive_helper.src", "r")

    # combine and write out
    print >> f, """%%{
%s

%s
%%}
%s

%s """ % (include.read(), enums.read(), grammar.read(), implemt.read())

    # close file handles
    include.close()
    enums.close()
    grammar.close()
    implemt.close()
