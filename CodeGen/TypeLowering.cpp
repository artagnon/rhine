#include "rhine/IR.h"
#include "rhine/Context.h"

namespace rhine {
llvm::Type *IntegerType::toLL(llvm::Module *M, Context *K) {
  switch (Bitwidth) {
  case 32:
    return K->Builder->getInt32Ty();
  case 64:
    return K->Builder->getInt64Ty();
  default:
    assert (0 && "int bitwidths other than 32 and 64 are unimplemented");
  }
}

llvm::Type *BoolType::toLL(llvm::Module *M, Context *K) {
  return K->Builder->getInt1Ty();
}

llvm::Type *FloatType::toLL(llvm::Module *M, Context *K) {
  return K->Builder->getFloatTy();
}

llvm::Type *StringType::toLL(llvm::Module *M, Context *K) {
  return K->Builder->getInt8PtrTy();
}

llvm::Type *FunctionType::toLL(llvm::Module *M, Context *K) {
  assert(0 && "first-class functions not yet implemented");
}

llvm::Value *Symbol::toLL(llvm::Module *M, Context *K) {
  assert(K && "null Symbol Table");
  return K->getMapping(getName(), getSourceLocation());
}

llvm::Value *GlobalString::toLL(llvm::Module *M, Context *K) {
  auto SRef = llvm::StringRef(getVal());
  return K->Builder->CreateGlobalStringPtr(SRef);
}

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
}
