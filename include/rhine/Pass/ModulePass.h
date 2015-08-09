//-*- C++ -*-
#ifndef RHINE_MODULETRANSFORM_H
#define RHINE_MODULETRANSFORM_H

#include "rhine/IR.h"

namespace rhine {
class ModulePass {
public:
  virtual ~ModulePass() {}
  virtual void runOnModule(Module *M) = 0;
};
}

#endif
