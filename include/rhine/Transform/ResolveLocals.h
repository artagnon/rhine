//-*- C++ -*-
#ifndef RHINE_RESOLVELOCALS_H
#define RHINE_RESOLVELOCALS_H

#include "rhine/Pass/FunctionPass.h"
#include "rhine/IR/Constant.h"

namespace rhine {
class ResolveLocals : public FunctionPass {
  Context *K;
public:
  ResolveLocals();
  virtual ~ResolveLocals();
  void runOnFunction(Function *F) override;
  void runOnModule(Module *M) override;
private:
  std::vector<BasicBlock *> getBlocksInScope(BasicBlock *BB);
  void lookupReplaceUse(std::string Name, Use &U,
                        BasicBlock *Block);
  void resolveOperandsOfUser(User *U, BasicBlock *BB);
  Value *lookupNameinBlock(std::string Name, BasicBlock *BB);
};
}

#endif
