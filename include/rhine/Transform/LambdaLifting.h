//-*- C++ -*-
#ifndef RHINE_LAMBDALIFTING_H
#define RHINE_LAMBDALIFTING_H

#include "rhine/IR.h"
#include "rhine/Transform/ModuleTransform.h"

namespace rhine {
class LambdaLifting : public ModuleTransform {
public:
  LambdaLifting(Context *K) : ModuleTransform(K) {}
  void runOnFunction(Function *F);
};
}

#endif
