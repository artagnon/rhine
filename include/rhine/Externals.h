//-*- C++ -*-

#ifndef RHINE_EXTERNALS_H
#define RHINE_EXTERNALS_H

#include "llvm/IR/Module.h"

namespace rhine {
class Prototype;
class PointerType;
class Context;

class Externals {
  Context *K;
  Prototype *PrintProto;
  Prototype *PrintlnProto;
  Prototype *MallocProto;
  Prototype *ToStringProto;

public:
  Externals(Context *K);
  static Externals *get(Context *K);
  Prototype *getMappingProto(std::string S);
  PointerType *getMappingTy(std::string S);
  llvm::Constant *getMappingVal(std::string S, llvm::Module *M);
};
}

#endif
