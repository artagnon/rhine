#pragma once

#include <vector>

#include "rhine/IR/Use.hpp"
#include "rhine/Pass/ModulePass.hpp"

namespace rhine {
class UnresolvedValue;
class BasicBlock;
class Context;

class Resolve : public ModulePass {
  Context *K;

public:
  Resolve();
  virtual ~Resolve();
  virtual void runOnModule(Module *M) override;

private:
  virtual void runOnFunction(Function *F) override;

  /// Replace an UnresolvedValue Use.
  void lookupReplaceUse(UnresolvedValue *V, Use &U, BasicBlock *Block);

  /// Dig through the Uses of a User and replace the UnresolvedValues.
  void resolveOperandsOfUser(User *U, BasicBlock *BB);
};
}
