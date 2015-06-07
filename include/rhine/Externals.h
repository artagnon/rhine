//-*- C++ -*-

#ifndef RHINE_EXTERNALS_H
#define RHINE_EXTERNALS_H

#include "llvm/IR/Module.h"
#include "rhine/IR/Type.h"

namespace rhine {
typedef llvm::Constant *ExternalsFTy(llvm::Module *M, Context *K);

struct ExternalsRef {
  PointerType *FTy;
  ExternalsFTy *FGenerator;
  ExternalsRef(PointerType *Ty, ExternalsFTy *H) : FTy(Ty), FGenerator(H) {}
};

struct Externals {
  static ExternalsFTy printf;
  static ExternalsFTy malloc;
  std::map<std::string, ExternalsRef> ExternalsMapping;
  PointerType *PrintfTyPtr;
  PointerType *MallocTyPtr;

  Externals(Context *K);
  static Externals *get(Context *K);
  PointerType *getMappingTy(std::string S);
  ExternalsFTy *getMappingVal(std::string S);
};
}

#endif
