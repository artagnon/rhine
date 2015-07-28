//-*- C++ -*-
#ifndef RHINE_LAMBDALIFTING_H
#define RHINE_LAMBDALIFTING_H

#include "rhine/IR.h"
#include "rhine/Transform/FunctionPass.h"

namespace rhine {
class LambdaLifting : public FunctionPass {
public:
  LambdaLifting(Context *K) : FunctionPass(K) {}
  void runOnFunction(Function *F) override;
};
}

#endif
