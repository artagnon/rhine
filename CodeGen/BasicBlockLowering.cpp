#include "rhine/Context.h"
#include "rhine/IR/Instruction.h"
#include "rhine/Externals.h"

namespace rhine {
llvm::Value *BasicBlock::toLL(llvm::Module *M, Context *K) {
  llvm::Value *LastLL;
  for (auto Val : ValueList)
    LastLL = Val->toLL(M, K);
  return LastLL;
}
}
