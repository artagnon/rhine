#include "rhine/Context.h"
#include "rhine/IR/Instruction.h"
#include "rhine/Externals.h"

namespace rhine {
llvm::BasicBlock *BasicBlock::toLL(llvm::Module *M, Context *K) {
  auto Ret = llvm::BasicBlock::Create(K->LLContext, Name);
  llvm::Value *LastLL;
  for (auto Val : ValueList)
    LastLL = Val->toLL(M, K);
  K->Builder->CreateRet(LastLL);
  return Ret;
}
}
