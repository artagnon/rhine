#pragma once

#include "llvm/IR/Module.h"

namespace rhine {
class Prototype;
class PointerType;
class Context;

class Externals {
  Prototype *PrintProto;
  Prototype *PrintlnProto;
  Prototype *MallocProto;
  Prototype *ToStringProto;

public:
  Externals(Context *K);
  virtual ~Externals();
  static Externals *get(Context *K);
  std::vector<Prototype *> getProtos() const;
  Prototype *getMappingProto(std::string S);
  llvm::Constant *getMappingVal(std::string S, llvm::Module *M);
};
}
