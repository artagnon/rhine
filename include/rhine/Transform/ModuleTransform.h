//-*- C++ -*-
#ifndef RHINE_MODULETRANSFORM_H
#define RHINE_MODULETRANSFORM_H

#include "rhine/IR.h"
#include "rhine/Transform/FunctionTransform.h"

namespace rhine {
class ModuleTransform : public FunctionTransform {
public:
  ModuleTransform(Context *K) : FunctionTransform(K) {}
  virtual void runOnModule(Module *M);
};
}

#endif
