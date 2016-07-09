#include "rhine/IR/Context.hpp"
#include "rhine/IR/Type.hpp"
#include "rhine/IR/Value.hpp"

namespace rhine {
llvm::Type *UnType::generate(llvm::Module *M) {
  assert(0 && "Cannot generate() without inferring type");
  return nullptr;
}

llvm::Type *VoidType::generate(llvm::Module *M) {
  return context()->Builder->getVoidTy();
}

llvm::Type *IntegerType::generate(llvm::Module *M) {
  switch (Bitwidth) {
  case 32:
    return context()->Builder->getInt32Ty();
  case 64:
    return context()->Builder->getInt64Ty();
  default:
    assert (0 && "int bitwidths other than 32 and 64 are unimplemented");
  }
  return nullptr;
}

llvm::Type *BoolType::generate(llvm::Module *M) {
  return context()->Builder->getInt1Ty();
}

llvm::Type *FloatType::generate(llvm::Module *M) {
  return context()->Builder->getFloatTy();
}

llvm::Type *StringType::generate(llvm::Module *M) {
  return context()->Builder->getInt8PtrTy();
}

llvm::Type *FunctionType::generate(llvm::Module *M) {
  auto ATys = getATys();
  std::vector<llvm::Type *> ATyAr;
  if (ATys.size() != 1 || !isa<VoidType>(ATys[0]))
    for (auto Ty: getATys())
      ATyAr.push_back(Ty->generate(M));
  return llvm::FunctionType::get(returnType()->generate(M), ATyAr, isVariadic());
}

llvm::Type *PointerType::generate(llvm::Module *M) {
  return llvm::PointerType::get(containedType()->generate(M), 0);
}
}
