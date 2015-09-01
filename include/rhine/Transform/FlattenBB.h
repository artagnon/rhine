// -*- C++ -*-
#ifndef RHINE_FLATTENBB_H
#define RHINE_FLATTENBB_H

#include "rhine/Pass/FunctionPass.h"

namespace rhine {
class BasicBlock;
class Context;

class FlattenBB : public FunctionPass {
  Context *K;
public:
  FlattenBB();
  virtual ~FlattenBB();
  void cleaveBB(BasicBlock *Cleavee, Function *Parent);
  virtual void runOnFunction(Function *F) override;
};
}

#endif
