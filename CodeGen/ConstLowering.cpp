#include "rhine/Context.h"
#include "rhine/IR/Constant.h"

namespace rhine {
llvm::Constant *ConstantInt::toLL(llvm::Module *M, Context *K) {
  auto LLTy = getType()->toLL(M, K);
  return llvm::ConstantInt::get(LLTy, getVal());
}

llvm::Constant *ConstantBool::toLL(llvm::Module *M, Context *K) {
  auto LLTy = getType()->toLL(M, K);
  return llvm::ConstantInt::get(LLTy, getVal());
}

llvm::Constant *ConstantFloat::toLL(llvm::Module *M, Context *K) {
  auto LLTy = getType()->toLL(M, K);
  return llvm::ConstantFP::get(LLTy, getVal());
}

llvm::Constant *Function::toLL(llvm::Module *M, Context *K) {
  auto Name = getName();
  auto RhFnTy = cast<FunctionType>(getType());
  auto FnTy = cast<llvm::FunctionType>(RhFnTy->toLL(M, K));
  llvm::Function *TheFunction;
  if (auto FunctionCandidate =
      dyn_cast<llvm::Function>(M->getOrInsertFunction(Name, FnTy)))
    TheFunction = FunctionCandidate;
  else {
    // TODO: Polymorphic functions
    K->DiagPrinter->errorReport(
        SourceLoc, Name + " was declared with different signature earlier");
    exit(1);
  }

  // Bind argument symbols to function argument values in symbol table
  auto ArgList = getArguments();
  auto S = ArgList.begin();
  auto V = TheFunction->arg_begin();
  auto SEnd = ArgList.end();
  auto VEnd = TheFunction->arg_end();
  for (; S != SEnd && V != VEnd; ++S, ++V)
    K->addMapping((*S)->getName(), nullptr, V);

  // Add function symbol to symbol table
  K->addMapping(Name, nullptr, TheFunction);

  BasicBlock *BB = BasicBlock::Create(K->Builder->getContext(), "entry", TheFunction);
  K->Builder->SetInsertPoint(BB);
  llvm::Value *LastLL;
  for (auto Val : getVal())
    LastLL = Val->toLL(M, K);
  K->Builder->CreateRet(LastLL);
  return TheFunction;
}
}
