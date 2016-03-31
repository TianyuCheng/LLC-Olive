set(LLVM_LINK_COMPONENTS
  ${LLVM_TARGETS_TO_BUILD}
  Analysis
  AsmPrinter
  CodeGen
  Core
  IRReader
  MC
  MIRParser
  ScalarOpts
  SelectionDAG
  Support
  Target
  TransformUtils
  )

# Support plugins.
set(LLVM_NO_DEAD_STRIP 1)

add_llvm_tool(llc-olive
  llc_olive.h
  llc_olive.cpp
  Tree.h
  Tree.cpp
  Value.h
  LiveRange.h
  FunctionState.cpp
  FunctionState.h
  RegisterAllocator.cpp
  RegisterAllocator.h
)
export_executable_symbols(llc-olive)
