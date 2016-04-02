# Makefile variables
TEST_DIR	:=./testcases
BIN_ROOT	:=../../build/bin
TOOL_ROOT	:=../../build/tools
CC			:=$(BIN_ROOT)/clang
OLIVE		:=./olive/olive
EXE			:=$(BIN_ROOT)/llc-olive

NUM_REGS  	:= 16
# colorful terminal output
RED  =`tput setaf 1`
GREEN=`tput setaf 2`
RESET=`tput sgr0`

assembly:=$(patsubst %.c,%.s,$(wildcard $(TEST_DIR)/*.c))
bitcodes:=$(patsubst %.c,%.bc,$(wildcard $(TEST_DIR)/*.c))

array: $(EXE) $(bitcodes)
	$(EXE) --num_regs=$(NUM_REGS) ./testcases/array.bc -o ./testcases/array.s

simpleSum: $(EXE) $(bitcodes)
	$(EXE) --num_regs=$(NUM_REGS) ./testcases/simpleSum.bc -o ./testcases/simpleSum.s

calculation:
	$(EXE) --num_regs=$(NUM_REGS) ./testcases/calculation.bc -o ./testcases/calculation.s

condition: $(EXE) $(bitcodes)
	$(EXE) --num_regs=$(NUM_REGS) ./testcases/condition.bc -o ./testcases/condition.s

for: $(EXE) $(bitcodes)
	$(EXE) --num_regs=$(NUM_REGS) ./testcases/forloop.bc -o ./testcases/forloop.s

while: $(EXE) $(bitcodes)
	$(EXE) --num_regs=$(NUM_REGS) ./testcases/while.bc -o ./testcases/while.s

function: $(EXE) $(bitcodes)
	$(EXE) --num_regs=$(NUM_REGS) ./testcases/function.bc -o ./testcases/function.s

$(EXE): llc_olive.cpp llc_olive.brg
	(cd $(TOOL_ROOT); make -j6)

llc_olive.cpp: llc_olive.brg llc_olive_helper.cxx llc_olive_helper.h
	$(OLIVE) $<

test: clean $(EXE) $(bitcodes) $(assembly)

%.s: %.bc
	$(EXE) --num_regs=$(NUM_REGS) $< -o $@

%.bc: %.c
	@$(CC) -O0 -emit-llvm $< -S -w -o $@

clean:
	@rm -rf $(EXE)
	@rm -rf $(TEST_DIR)/*.log
	@rm -rf $(TEST_DIR)/.*.swp
	@rm -rf ./llc_olive.h ./llc_olive.cpp

.PHONY: opt compile tar test run push clean
