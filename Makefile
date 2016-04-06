# Makefile variables
TEST_DIR	:=./testcases
BIN_ROOT	:=../../build/bin
TOOL_ROOT	:=../../build/tools
CC			:=$(BIN_ROOT)/clang
OLIVE		:=./olive/olive
EXE			:=$(BIN_ROOT)/llc-olive

NUM_REGS  	:= 3
# colorful terminal output
RED  =`tput setaf 1`
GREEN=`tput setaf 2`
RESET=`tput sgr0`

assembly:=$(patsubst %.c,%.s,$(wildcard $(TEST_DIR)/*.c))
bitcodes:=$(patsubst %.c,%.bc,$(wildcard $(TEST_DIR)/*.c))

$(EXE): llc_olive.cpp llc_olive.brg
	(cd $(TOOL_ROOT); make -j6)

tar:
	tar -zcvf ./assignment6.tar.gz \
		Makefile olive/* testcases/* CMakeLists.txt \
		README.md \
		LiveRange.h \
		Insts.h Value.h \
		Tree.h Tree.cpp \
		FunctionState.h FunctionState.cpp \
		RegisterAllocator.h RegisterAllocator.cpp \
		llc_olive_helper.h llc_olive_helper.cxx llc_olive.brg

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

llc_olive.cpp: llc_olive.brg llc_olive_helper.cxx llc_olive_helper.h
	$(OLIVE) $<

test: clean $(EXE) $(bitcodes) $(assembly)

%.s: %.bc
	$(EXE) --num_regs=$(NUM_REGS) $< -o $@

%.bc: %.c
	@$(CC) -O0 -emit-llvm $< -S -w -o $@

clean:
	@rm -rf $(EXE)
	@rm -rf $(TEST_DIR)/*.s
	@rm -rf $(TEST_DIR)/*.log
	@rm -rf $(TEST_DIR)/.*.swp
	@rm -rf ./assignment6.tar.gz
	@rm -rf ./llc_olive.h ./llc_olive.cpp

.PHONY: opt compile tar test run push clean
