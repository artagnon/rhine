//-*- C++ -*-
#ifndef RHINE_MODULETRANSFORM_H
#define RHINE_MODULETRANSFORM_H

#include "rhine/IR.h"

namespace rhine {
class ModuleTransform {
protected:
  Context *K;
public:
  ModuleTransform(Context *K) : K(K) {}
  virtual void runOnModule(Module *M) = 0;
};
}

#endif
