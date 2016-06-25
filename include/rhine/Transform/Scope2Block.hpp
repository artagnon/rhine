#pragma once

#include "rhine/Pass/FunctionPass.hpp"

namespace rhine {
class BasicBlock;
class Context;

/// Convert scopes nested within one another into BasicBlocks.
class Scope2Block : public FunctionPass {
  Context *K;

public:
  Scope2Block();
  virtual ~Scope2Block() = default;

  /// Look for branch instructions in the BasicBlock (actually a scope before
  /// transformation), and use them to truncate the Cleavee, and generate three
  /// more blocks: TrueBB, FalseBB, and MergeBB. An edge from MergeBB to
  /// ReturnTo is created to return control to the main scope (nullptr in the
  /// case of the EntryBlock scope).
  void cleaveBlockAtBranches(BasicBlock *Cleavee,
                             BasicBlock *ReturnTo = nullptr);

  /// Validates that there are no early returns or block terminators.
  void validateBlockForm(BasicBlock *BB);

  /// Sets up the Context and calls the subroutines.
  virtual void runOnFunction(Function *F) override;
};
}
