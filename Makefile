# Makefile variables
TEST_DIR	:=./testcases
BIN_ROOT	:=../../build/bin
TOOL_ROOT	:=../../build/tools
CC			:=$(BIN_ROOT)/clang
OLIVE		:=./olive/olive
EXE			:=$(BIN_ROOT)/llc-olive

# colorful terminal output
RED  =`tput setaf 1`
GREEN=`tput setaf 2`
RESET=`tput sgr0`

assembly:=$(patsubst %.c,%.s,$(wildcard $(TEST_DIR)/*.c))
bitcodes:=$(patsubst %.c,%.bc,$(wildcard $(TEST_DIR)/*.c))

run: $(EXE) $(bitcodes)
	@echo "=================================================================="
	$(EXE) --num_regs=16 ./testcases/simpleSum.bc -o ./testcases/simpleSum.s
	@echo "------------------------------------------------------------------"
	$(EXE) --num_regs=16 ./testcases/calculation.bc -o ./testcases/calculation.s
	@echo "------------------------------------------------------------------"
	$(EXE) --num_regs=16 ./testcases/condition.bc -o ./testcases/condition.s

$(EXE): llc_olive.cpp llc_olive.brg
	(cd $(TOOL_ROOT); make -j6) ; rm llc_olive.brg

llc_olive.cpp: llc_olive.brg
	$(OLIVE) $<

llc_olive.brg: llc_olive.py
	./$<

test: clean $(EXE) $(bitcodes) $(assembly)

%.s: %.bc
	$(EXE) --num_regs=32 $< -o $@

%.bc: %.c
	@$(CC) -O0 -emit-llvm $< -S -o $@

clean:
	@rm -rf $(EXE)
	@rm -rf $(TEST_DIR)/*.bc
	@rm -rf $(TEST_DIR)/*.log
	@rm -rf $(TEST_DIR)/.*.swp
	@rm -rf ./llc_olive.h ./llc_olive.cpp ./llc_olive.brg

.PHONY: compile tar test run push clean
