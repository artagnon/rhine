#include "rhine/IR/Constant.hpp"
#include "rhine/Externals.hpp"
#include "rhine/IR/Context.hpp"

namespace rhine {
llvm::Constant *ConstantInt::toLL(llvm::Module *M) {
  auto LLTy = type()->toLL(M);
  return llvm::ConstantInt::get(LLTy, val());
}

llvm::Constant *ConstantBool::toLL(llvm::Module *M) {
  auto LLTy = type()->toLL(M);
  return llvm::ConstantInt::get(LLTy, val());
}

llvm::Constant *ConstantFloat::toLL(llvm::Module *M) {
  auto LLTy = type()->toLL(M);
  return llvm::ConstantFP::get(LLTy, val());
}

llvm::Constant *Pointer::toLL(llvm::Module *M) {
  auto K = context();
  auto Name = val()->getName();
  if (auto Val = val()->getLoweredValue())
    return cast<llvm::Constant>(Val);
  return Externals::get(K)->mappingVal(Name, M);
}
}
