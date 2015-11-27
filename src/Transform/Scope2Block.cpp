#include "rhine/Diagnostic/Diagnostic.h"
#include "rhine/IR/BasicBlock.h"
#include "rhine/IR/Constant.h"
#include "rhine/IR/Context.h"
#include "rhine/IR/Instruction.h"
#include "rhine/Transform/Scope2Block.h"

namespace rhine {
class Value;

Scope2Block::Scope2Block() : K(nullptr) {}

Scope2Block::~Scope2Block() {}

void Scope2Block::cleaveBlockAtBranches(BasicBlock *Cleavee) {
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
  Cleavee->StmList.erase(StartInst, Cleavee->end());

  /// Set up predecessors and successors.
  Cleavee->addSuccessors({TrueBlock, FalseBlock});
  TrueBlock->addPredecessors({Cleavee});
  TrueBlock->setParent(Parent);
  FalseBlock->addPredecessors({Cleavee});
  FalseBlock->setParent(Parent);
  TrueBlock->addSuccessors({MergeBlock});
  FalseBlock->addSuccessors({MergeBlock});
  MergeBlock->addPredecessors({TrueBlock, FalseBlock});
  MergeBlock->setParent(Parent);

  /// Re-populate Parent.
  Parent->push_back(TrueBlock);
  Parent->push_back(FalseBlock);
  Parent->push_back(MergeBlock);
  return cleaveBlockAtBranches(MergeBlock);
}

void Scope2Block::validateBlockForm(BasicBlock *BB) {
  if (BB->begin() == BB->end())
    return;
  std::vector<Instruction *>::iterator It;
  for (It = BB->begin(); std::next(It) != BB->end(); ++It)
    if (dyn_cast<TerminatorInst>(*It) || dyn_cast<ReturnInst>(*It)) {
      K->DiagPrinter->errorReport((*It)->getSourceLocation(),
                                  "unexpected block terminator or return");
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
