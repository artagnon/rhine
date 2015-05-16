//-*- C++ -*-

#ifndef EXTERNALS_H
#define EXTERNALS_H

#include "llvm/IR/Constant.h"

namespace rhine {
typedef llvm::Constant *ExternalsFTy(llvm::Module *M, Context *K,
                                     location &SourceLoc);

struct Externals {
  static ExternalsFTy printf;
  static ExternalsFTy malloc;

  static std::map<std::string, ExternalsFTy *> ExternalsMapping;
  static ExternalsFTy *getMapping(std::string S) {
    auto V = ExternalsMapping.find(S);
    if (V == ExternalsMapping.end())
      return nullptr;
    return V->second;
  }
};
}

#endif
