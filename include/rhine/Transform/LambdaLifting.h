//-*- C++ -*-
#ifndef RHINE_LAMBDALIFTING_H
#define RHINE_LAMBDALIFTING_H

#include "rhine/IR.h"

namespace rhine {
class LambdaLifting {
  Context *K;
public:
  LambdaLifting(Context *K) : K(K) {}
  void runOnFunction(Function *F);
  void runOnModule(Module *M);
};
}

#endif
