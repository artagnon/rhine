#include "rhine/IR/Context.h"
#include "rhine/IR/Instruction.h"
#include "rhine/IR/Constant.h"
#include "rhine/Externals.h"

namespace rhine {
llvm::Value *BasicBlock::toValuesLL(llvm::Module *M) {
  llvm::Value *LastLL;
  for (auto Val : StmList)
    LastLL = Val->toLL(M);
  return LastLL;
}

llvm::BasicBlock *BasicBlock::toContainerLL(llvm::Module *M) {
  auto K = getContext();
  auto ParentF = cast<llvm::Function>(K->Map.getl(Parent));
  auto Ret = llvm::BasicBlock::Create(K->LLContext, Name, ParentF);
  K->Builder->SetInsertPoint(Ret);
  return Ret;
}

llvm::Value *BasicBlock::toLL(llvm::Module *M) {
  toContainerLL(M);
  return toValuesLL(M);
}
}
