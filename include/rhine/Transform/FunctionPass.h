//-*- C++ -*-
#ifndef RHINE_FUNCTIONTRANSFORM_H
#define RHINE_FUNCTIONTRANSFORM_H

#include "rhine/IR.h"
#include "rhine/Transform/ModulePass.h"

namespace rhine {
class FunctionPass : public ModulePass {
public:
  FunctionPass(Context *K) : ModulePass(K) {}
  void runOnModule(Module *M);
  virtual void runOnFunction(Function *F) = 0;
};
}

#endif
