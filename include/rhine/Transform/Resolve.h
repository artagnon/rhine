//-*- C++ -*-
#ifndef RHINE_RESOLVELOCALS_H
#define RHINE_RESOLVELOCALS_H

#include "rhine/Pass/FunctionPass.h"

namespace rhine {
class UnresolvedValue;

class Resolve : public FunctionPass {
  Context *K;
public:
  Resolve();
  virtual ~Resolve();
  void runOnFunction(Function *F) override;
  void runOnModule(Module *M) override;
private:
  std::vector<BasicBlock *> getBlocksInScope(BasicBlock *BB);
  void lookupReplaceUse(UnresolvedValue *V, Use &U,
                        BasicBlock *Block);
  void resolveOperandsOfUser(User *U, BasicBlock *BB);
  Value *lookupNameinBlock(Value *V, BasicBlock *BB);
};
}

#endif
