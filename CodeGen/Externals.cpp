#include "rhine/IR.h"
#include "rhine/Externals.h"

#include "llvm/IR/Type.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ExecutionEngine/MCJIT.h"

using namespace llvm;

namespace rhine {
llvm::Function *Externals::printf(llvm::Module *M) {
  auto ArgTys = llvm::ArrayRef<llvm::Type *>(RhBuilder.getInt8PtrTy());
  llvm::FunctionType* printf_type =
    llvm::FunctionType::get(RhBuilder.getInt32Ty(), ArgTys, true);
  auto Handle =
    M->getOrInsertFunction("printf", printf_type);
  if (auto HandleF = dyn_cast<llvm::Function>(Handle))
    return HandleF;
  assert(0 && "Prototype previously declared with wrong signature");
}

llvm::Function *Externals::malloc(llvm::Module *M) {
  auto ArgTys = llvm::ArrayRef<llvm::Type *>(RhBuilder.getInt64Ty());
  llvm::FunctionType* malloc_type =
    llvm::FunctionType::get(RhBuilder.getInt8PtrTy(), ArgTys, false);
  auto Handle =
    M->getOrInsertFunction("malloc", malloc_type);
  if (auto HandleF = dyn_cast<llvm::Function>(Handle))
    return HandleF;
  assert(0 && "Prototype previously declared with wrong signature");
}
}
