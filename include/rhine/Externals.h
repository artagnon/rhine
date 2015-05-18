//-*- C++ -*-

#ifndef EXTERNALS_H
#define EXTERNALS_H

#include "llvm/IR/Module.h"
#include "rhine/IR/Type.h"

namespace rhine {
typedef llvm::Constant *ExternalsFTy(llvm::Module *M, Context *K);

struct ExternalsRef {
  FunctionType *FTy;
  ExternalsFTy *FGenerator;
  ExternalsRef(FunctionType *Ty, ExternalsFTy *H) : FTy(Ty), FGenerator(H) {}
};

struct Externals {
  Context *K;
  static ExternalsFTy printf;
  static ExternalsFTy malloc;
  std::map<std::string, ExternalsRef> ExternalsMapping;

  Externals(Context *K);
  static Externals *get(Context *K);
  FunctionType *getMappingTy(std::string S);
  ExternalsFTy *getMappingVal(std::string S);
};
}

#endif
