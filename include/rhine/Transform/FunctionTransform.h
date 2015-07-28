//-*- C++ -*-
#ifndef RHINE_FUNCTIONTRANSFORM_H
#define RHINE_FUNCTIONTRANSFORM_H

#include "rhine/IR.h"

namespace rhine {
class FunctionTransform {
protected:
  Context *K;
public:
  FunctionTransform(Context *K) : K(K) {}
  virtual void runOnFunction(Function *F) = 0;
};
}

#endif
