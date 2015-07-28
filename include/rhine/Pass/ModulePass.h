//-*- C++ -*-
#ifndef RHINE_MODULETRANSFORM_H
#define RHINE_MODULETRANSFORM_H

#include "rhine/IR.h"

namespace rhine {
class ModulePass {
protected:
  Context *K;
public:
  ModulePass(Context *K) : K(K) {}
  virtual ~ModulePass() {}
  virtual void runOnModule(Module *M) = 0;
};
}

#endif
