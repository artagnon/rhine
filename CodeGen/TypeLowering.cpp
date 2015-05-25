#include "rhine/Context.h"
#include "rhine/IR/Value.h"

namespace rhine {
llvm::Type *UnType::toLL(llvm::Module *M, Context *K) {
  assert(0 && "Cannot toLL() without inferring type");
}

llvm::Type *VoidType::toLL(llvm::Module *M, Context *K) {
  return K->Builder->getVoidTy();
}

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
  // TODO: Support variadic functiontype
  auto ATys = getATys();
  std::vector<llvm::Type *> ATyAr;
  if (ATys.size() != 1 || !VoidType::classof(ATys[0]))
    for (auto Ty: getATys())
      ATyAr.push_back(Ty->toLL(M, K));
  return llvm::FunctionType::get(getRTy()->toLL(M, K), ATyAr, false);
}

llvm::Type *PointerType::toLL(llvm::Module *M, Context *K) {
  return llvm::PointerType::get(getCTy()->toLL(M, K), 0);
}
}
