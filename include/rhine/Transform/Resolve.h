//-*- C++ -*-
#ifndef RHINE_RESOLVELOCALS_H
#define RHINE_RESOLVELOCALS_H

#include "rhine/Pass/ModulePass.h"

namespace rhine {
class UnresolvedValue;

class Resolve : public ModulePass {
  Context *K;
public:
  Resolve();
  virtual ~Resolve();
  virtual void runOnModule(Module *M) override;
private:
  virtual void runOnFunction(Function *F) override;
  std::vector<BasicBlock *> getBlocksInScope(BasicBlock *BB);
  void lookupReplaceUse(UnresolvedValue *V, Use &U);
  void resolveOperandsOfUser(User *U);
  Value *lookupNameinBlock(Value *V, BasicBlock *B);
};
}

#endif
