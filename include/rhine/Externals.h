//-*- C++ -*-

#ifndef RHINE_EXTERNALS_H
#define RHINE_EXTERNALS_H

#include "llvm/IR/Module.h"
#include "rhine/IR/Type.h"

namespace rhine {
struct Externals;

typedef llvm::Constant *(Externals::*ExternalsFTy)(llvm::Module *M);

struct ExternalsRef {
  PointerType *FTy;
  ExternalsFTy FHandle;
  ExternalsRef(PointerType *Ty, ExternalsFTy H) : FTy(Ty), FHandle(H) {}
};

struct Externals {
  Context *K;
  FunctionType *PrintTy;
  FunctionType *MallocTy;
  FunctionType *ToStringTy;

  llvm::Constant *print(llvm::Module *M);
  llvm::Constant *println(llvm::Module *M);
  llvm::Constant *malloc(llvm::Module *M);
  llvm::Constant *toString(llvm::Module *M);

  std::map<std::string, ExternalsRef> ExternalsMapping;

  Externals(Context *K_);
  static Externals *get(Context *K);
  PointerType *getMappingTy(std::string S);
  llvm::Constant *getMappingVal(std::string S, llvm::Module *M);
};
}

#endif
