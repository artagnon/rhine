//-*- C++ -*-
#ifndef RHINE_FUNCTIONTRANSFORM_H
#define RHINE_FUNCTIONTRANSFORM_H

#include "rhine/Pass/ModulePass.h"

namespace rhine {
class Module;
class Function;

class FunctionPass : public ModulePass {
public:
  void runOnModule(Module *M);
  virtual void runOnFunction(Function *F) = 0;
};
}

#endif
