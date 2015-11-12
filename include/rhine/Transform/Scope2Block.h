// -*- C++ -*-
#ifndef RHINE_Scope2Block_H
#define RHINE_Scope2Block_H

#include "rhine/Pass/FunctionPass.h"

namespace rhine {
class BasicBlock;
class Context;

class Scope2Block : public FunctionPass {
  Context *K;
public:
  Scope2Block();
  virtual ~Scope2Block();
  void cleaveBB(BasicBlock *Cleavee, Function *Parent);
  virtual void runOnFunction(Function *F) override;
};
}

#endif
