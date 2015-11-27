// -*- C++ -*-
#ifndef RHINE_Scope2Block_H
#define RHINE_Scope2Block_H

#include "rhine/Pass/FunctionPass.h"

namespace rhine {
class BasicBlock;
class Context;

/// Convert scopes nested within one another into BasicBlocks.
class Scope2Block : public FunctionPass {
  Context *K;
public:
  Scope2Block();
  virtual ~Scope2Block();

  /// Look for branch instructions in the BasicBlock, and use them to cleave the
  /// Scope.
  void cleaveBlockAtBranches(BasicBlock *Cleavee);

  /// Validates that there are no early returns or block terminators.
  void validateBlockForm(BasicBlock *BB);

  /// Sets up the Context and calls the subroutines.
  virtual void runOnFunction(Function *F) override;
};
}

#endif
