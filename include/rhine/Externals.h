//-*- C++ -*-

#ifndef EXTERNALS_H
#define EXTERNALS_H

#include "llvm/IR/Module.h"

namespace rhine {
typedef llvm::Constant *ExternalsFTy(llvm::Module *M, Context *K);

struct Externals {
  static ExternalsFTy printf;
  static ExternalsFTy malloc;
  std::map<std::string, ExternalsFTy *> ExternalsMapping;

  Externals();
  static Externals *get();
  ExternalsFTy *getMapping(std::string S);
};
}

#endif
