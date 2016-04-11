# Makefile variables
TEST_DIR	:=./testcases
BIN_ROOT	:=../../build/bin
TOOL_ROOT	:=../../build/tools
CC			:=$(BIN_ROOT)/clang
OPT			:=$(BIN_ROOT)/opt
OLIVE		:=./olive/olive
EXE			:=$(BIN_ROOT)/llc-olive

NUM_REGS  	:= 6
# colorful terminal output
RED  =`tput setaf 1`
GREEN=`tput setaf 2`
RESET=`tput sgr0`

assembly:=$(patsubst %.c,%.s, $(wildcard $(TEST_DIR)/*.c))
bitcodes:=$(patsubst %.c,%.bc,$(wildcard $(TEST_DIR)/*.c))
targets :=$(patsubst %.c,%,   $(shell (cd testcases; ls *.c)))

$(EXE): llc_olive.brg llc_olive.cpp FunctionState.cpp Tree.cpp RegisterAllocator.cpp
	(cd $(TOOL_ROOT); make -j6)

tar:
	(cd ../; tar -zcvf ./assignment6.tar.gz llc-olive/*)

%: $(EXE) ./testcases/%.s
	@echo -ne "${GREEN}[ ] TESTING $@ ... ${RESET}"
	@$(CC) ./testcases/$@.s -o ./testcases/$@.actual > /tmp/$@.compile
	@$(CC) ./testcases/$@.bc -o ./testcases/$@.expected >/dev/null 2>/dev/null
	@./test.sh $@

llc_olive.cpp: llc_olive.brg
	@$(OLIVE) $<

test: $(EXE) $(bitcodes) $(targets)

%.s: %.bc
	@$(EXE) --num_regs=$(NUM_REGS) $< -o $@

%.bc: %.c
	@$(CC) -O0 -emit-llvm $< -S -c -o $@
	@#$(CC) -O0 -emit-llvm $< -S -c -o $@.tmp
	@#$(OPT) -mem2reg $@.tmp -S -o $@
	@#rm -rf $@.tmp

clean:
	@rm -rf $(EXE)
	@rm -rf $(TEST_DIR)/*.actual
	@rm -rf $(TEST_DIR)/*.expected
	@rm -rf $(TEST_DIR)/*.s
	@rm -rf $(TEST_DIR)/*.bc
	@rm -rf $(TEST_DIR)/*.log
	@rm -rf $(TEST_DIR)/.*.swp
	@rm -rf $(TEST_DIR)/*.tmp
	@rm -rf ./assignment6.tar.gz

stampede: clean tar
	@scp ../CMakeLists.txt tcheng@stampede.tacc.utexas.edu:/work/03741/tcheng/llvm/tools/CMakeLists.txt
	@scp ../assignment6.tar.gz tcheng@stampede.tacc.utexas.edu:/work/03741/tcheng/llvm/tools/
	@ssh tcheng@stampede.tacc.utexas.edu 'cd /work/03741/tcheng/llvm/tools/; rm -rf llc-olive; tar xvf ./assignment6.tar.gz; cd llc-olive; make clean test'

.PHONY: opt compile tar test run push clean stampede
.PRECIOUS: %.bc %.s
