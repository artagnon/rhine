#include "rhine/IR/Context.hpp"
#include "rhine/IR/Constant.hpp"
#include "rhine/Externals.hpp"

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

llvm::Constant *Pointer::toLL(llvm::Module *M) {
  auto K = getContext();
  auto Name = getVal()->getName();
  if (auto Val = K->Map.getl(getVal()))
    return cast<llvm::Constant>(Val);
  return Externals::get(K)->getMappingVal(Name, M);
}
}
