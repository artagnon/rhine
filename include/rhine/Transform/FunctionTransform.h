//-*- C++ -*-
#ifndef RHINE_FUNCTIONTRANSFORM_H
#define RHINE_FUNCTIONTRANSFORM_H

#include "rhine/IR.h"
#include "rhine/Transform/ModuleTransform.h"

namespace rhine {
class FunctionTransform : public ModuleTransform {
public:
  FunctionTransform(Context *K) : ModuleTransform(K) {}
  void runOnModule(Module *M);
  virtual void runOnFunction(Function *F) = 0;
};
}

#endif
