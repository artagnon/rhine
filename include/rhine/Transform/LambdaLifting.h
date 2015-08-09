//-*- C++ -*-
#ifndef RHINE_LAMBDALIFTING_H
#define RHINE_LAMBDALIFTING_H

#include "rhine/IR.h"
#include "rhine/Pass/FunctionPass.h"

namespace rhine {
class LambdaLifting : public FunctionPass {
public:
  virtual ~LambdaLifting() {}
  void runOnFunction(Function *F) override;
};
}

#endif
