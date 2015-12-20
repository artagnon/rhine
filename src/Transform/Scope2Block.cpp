#include "rhine/Diagnostic/Diagnostic.hpp"
#include "rhine/IR/BasicBlock.hpp"
#include "rhine/IR/Constant.hpp"
#include "rhine/IR/Context.hpp"
#include "rhine/IR/Function.hpp"
#include "rhine/IR/Instruction.hpp"
#include "rhine/Transform/Scope2Block.hpp"

namespace rhine {
class Value;

Scope2Block::Scope2Block() : K(nullptr) {}

Scope2Block::~Scope2Block() {}

void Scope2Block::cleaveBlockAtBranches(BasicBlock *Cleavee,
                                        BasicBlock *ReturnTo) {
  auto Parent = Cleavee->getParent();
  auto It = std::find_if(Cleavee->begin(), Cleavee->end(),
                         [](Instruction *Arg) { return isa<IfInst>(Arg); });
  if (It == Cleavee->end())
    return;
  auto BranchInst = cast<IfInst>(*It);
  auto TrueBlock = BranchInst->getTrueBB();
  auto FalseBlock = BranchInst->getFalseBB();
  auto StartInst = std::next(It);
  auto MergeBlock = BasicBlock::get(
      "exit", std::vector<Instruction *>(StartInst, Cleavee->end()), K);

  /// Remove everything from the branch to the end of the Block.
  Cleavee->getInstList().erase(StartInst, Cleavee->end());

  /// Set up predecessors and successors.
  Cleavee->setSuccessors({TrueBlock, FalseBlock});

  TrueBlock->setPredecessors({Cleavee});
  TrueBlock->setParent(Parent);
  TrueBlock->setSuccessors({MergeBlock});

  FalseBlock->setPredecessors({Cleavee});
  FalseBlock->setParent(Parent);
  FalseBlock->setSuccessors({MergeBlock});

  MergeBlock->setPredecessors({TrueBlock, FalseBlock});
  MergeBlock->setParent(Parent);
  if (ReturnTo)
    MergeBlock->setSuccessors({ReturnTo});

  /// Create three new blocks in the function, inserting and cleaving them as we
  /// go. Program correctness isn't dependent on insertion order, but
  /// pretty-printing (and tests) will look strange without the right ordering.
  Parent->push_back(TrueBlock);
  cleaveBlockAtBranches(TrueBlock, MergeBlock);
  Parent->push_back(FalseBlock);
  cleaveBlockAtBranches(FalseBlock, MergeBlock);
  Parent->push_back(MergeBlock);
  cleaveBlockAtBranches(MergeBlock, MergeBlock);
}

void Scope2Block::validateBlockForm(BasicBlock *BB) {
  if (BB->begin() == BB->end())
    return;
  std::vector<Instruction *>::iterator It;
  for (It = BB->begin(); std::next(It) != BB->end(); ++It)
    if (dyn_cast<TerminatorInst>(*It) || dyn_cast<ReturnInst>(*It)) {
      DiagnosticPrinter((*It)->getSourceLocation())
          << "unexpected block terminator";
      exit(1);
    }
}

void Scope2Block::runOnFunction(Function *F) {
  K = F->getContext();
  auto EntryBlock = F->getEntryBlock();
  EntryBlock->setParent(F);
  cleaveBlockAtBranches(EntryBlock);
  for (auto BB : *F)
    validateBlockForm(BB);
}
}
