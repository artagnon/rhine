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
  auto FType = dyn_cast<FunctionType>(getType());
  auto RType = FType->getRTy()->toLL(M, K);
  std::vector<llvm::Type *> ArgTys;
  for (auto RhTy: FType->getATys())
    ArgTys.push_back(RhTy->toLL(M, K));
  auto ArgTyAr = makeArrayRef(ArgTys);
  auto TheFunctionTy = llvm::FunctionType::get(RType, ArgTyAr, false);
  llvm::Function *TheFunction;
  if (auto FunctionCandidate =
      dyn_cast<llvm::Function>(M->getOrInsertFunction(Name, TheFunctionTy)))
    TheFunction = FunctionCandidate;
  else {
    // TODO: Polymorphic functions
    K->DiagPrinter->errorReport(
        SourceLoc, Name + " was declared with different signature earlier");
    exit(1);
  }

  // Bind argument symbols to function argument values in symbol table
  auto ArgList = getArgumentList();
  auto S = ArgList.begin();
  auto V = TheFunction->arg_begin();
  auto SEnd = ArgList.end();
  auto VEnd = TheFunction->arg_end();
  for (; S != SEnd && V != VEnd; ++S, ++V)
    K->addMapping((*S)->getName(), V);

  // Add function symbol to symbol table
  K->addMapping(Name, TheFunction);

  BasicBlock *BB = BasicBlock::Create(K->Builder->getContext(), "entry", TheFunction);
  K->Builder->SetInsertPoint(BB);
  llvm::Value *LastLL;
  for (auto Val : getVal())
    LastLL = Val->toLL(M, K);
  K->Builder->CreateRet(LastLL);
  return TheFunction;
}
}
