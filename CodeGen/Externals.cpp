#include "rhine/IR.h"
#include "rhine/Externals.h"

#include "llvm/IR/Type.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ExecutionEngine/MCJIT.h"

using namespace llvm;

namespace rhine {
llvm::Function *Externals::printf(llvm::Module *M) {
  // We don't to generate two printf functions (printf, printf1) for the same
  // module; for different modules, they're different functions
  static llvm::Module *ThisModule = nullptr;
  static llvm::Function *Handle = nullptr;
  if (ThisModule == M && Handle) return Handle;
  ThisModule = M;

  auto ArgTys = llvm::ArrayRef<llvm::Type *>(RhBuilder.getInt8PtrTy());
  llvm::FunctionType* printf_type =
    llvm::FunctionType::get(RhBuilder.getInt32Ty(), ArgTys, true);

  Handle =
    llvm::Function::Create(printf_type, llvm::Function::ExternalLinkage,
                           Twine("printf"), M);
  Handle->setCallingConv(llvm::CallingConv::C);
  return Handle;
}

llvm::Function *Externals::malloc(llvm::Module *M) {
  static llvm::Module *ThisModule = nullptr;
  static llvm::Function *Handle = nullptr;
  if (ThisModule == M && Handle) return Handle;
  ThisModule = M;

  auto ArgTys = llvm::ArrayRef<llvm::Type *>(RhBuilder.getInt64Ty());
  llvm::FunctionType* malloc_type =
    llvm::FunctionType::get(RhBuilder.getInt8PtrTy(), ArgTys, false);

  Handle =
    llvm::Function::Create(malloc_type, llvm::Function::ExternalLinkage,
                           Twine("malloc"), M);
  Handle->setCallingConv(llvm::CallingConv::C);
  return Handle;
}
}
