#include "rhine/Context.h"
#include "rhine/IR/Constant.h"

namespace rhine {
llvm::Constant *ConstantInt::toLL(llvm::Module *M) {
  auto LLTy = getType()->toLL(M);
  return llvm::ConstantInt::get(LLTy, getVal());
}

llvm::Constant *ConstantBool::toLL(llvm::Module *M) {
  auto LLTy = getType()->toLL(M);
  return llvm::ConstantInt::get(LLTy, getVal());
}

llvm::Constant *ConstantFloat::toLL(llvm::Module *M) {
  auto LLTy = getType()->toLL(M);
  return llvm::ConstantFP::get(LLTy, getVal());
}

llvm::Constant *Function::toLL(llvm::Module *M) {
  auto K = getContext();
  auto Name = getName();
  auto RhFnTy = cast<FunctionType>(getType());
  auto FnTy = cast<llvm::FunctionType>(RhFnTy->toLL(M));
  if (auto FunctionCandidate =
      dyn_cast<llvm::Function>(M->getOrInsertFunction(Name, FnTy)))
    K->CurrentFunction = FunctionCandidate;
  else {
    K->DiagPrinter->errorReport(
        SourceLoc, Name + " was declared with different signature earlier");
    exit(1);
  }
  K->CurrentFunction->setGC("rhgc");

  // Bind argument symbols to function argument values in symbol table
  auto ArgList = getArguments();
  auto S = ArgList.begin();
  auto V = K->CurrentFunction->arg_begin();
  auto SEnd = ArgList.end();
  auto VEnd = K->CurrentFunction->arg_end();
  for (; S != SEnd && V != VEnd; ++S, ++V)
    K->addMapping((*S)->getName(), nullptr, V);

  // Add function symbol to symbol table
  K->addMapping(Name, nullptr, K->CurrentFunction);

  llvm::BasicBlock *BB =
    llvm::BasicBlock::Create(K->Builder->getContext(),
                             "entry", K->CurrentFunction);
  K->Builder->SetInsertPoint(BB);
  auto LastLL = getVal()->toLL(M);
  K->Builder->CreateRet(LastLL);
  return K->CurrentFunction;
}
}
