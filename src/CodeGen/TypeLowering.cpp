#include "rhine/IR/Context.hpp"
#include "rhine/IR/Type.hpp"
#include "rhine/IR/Value.hpp"

namespace rhine {
llvm::Type *UnType::toLL(llvm::Module *M) {
  assert(0 && "Cannot toLL() without inferring type");
  return nullptr;
}

llvm::Type *VoidType::toLL(llvm::Module *M) {
  return getContext()->Builder->getVoidTy();
}

llvm::Type *IntegerType::toLL(llvm::Module *M) {
  switch (Bitwidth) {
  case 32:
    return getContext()->Builder->getInt32Ty();
  case 64:
    return getContext()->Builder->getInt64Ty();
  default:
    assert (0 && "int bitwidths other than 32 and 64 are unimplemented");
  }
  return nullptr;
}

llvm::Type *BoolType::toLL(llvm::Module *M) {
  return getContext()->Builder->getInt1Ty();
}

llvm::Type *FloatType::toLL(llvm::Module *M) {
  return getContext()->Builder->getFloatTy();
}

llvm::Type *StringType::toLL(llvm::Module *M) {
  return getContext()->Builder->getInt8PtrTy();
}

llvm::Type *FunctionType::toLL(llvm::Module *M) {
  auto ATys = getATys();
  std::vector<llvm::Type *> ATyAr;
  if (ATys.size() != 1 || !isa<VoidType>(ATys[0]))
    for (auto Ty: getATys())
      ATyAr.push_back(Ty->toLL(M));
  return llvm::FunctionType::get(getRTy()->toLL(M), ATyAr, isVariadic());
}

llvm::Type *PointerType::toLL(llvm::Module *M) {
  return llvm::PointerType::get(getCTy()->toLL(M), 0);
}
}
