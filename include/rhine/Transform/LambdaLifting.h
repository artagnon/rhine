//-*- C++ -*-
#ifndef RHINE_LAMBDALIFTING_H
#define RHINE_LAMBDALIFTING_H

#include "rhine/IR.h"
#include "rhine/Transform/FunctionTransform.h"

namespace rhine {
class LambdaLifting : public FunctionTransform {
public:
  LambdaLifting(Context *K) : FunctionTransform(K) {}
  void runOnFunction(Function *F) override;
};
}

#endif
