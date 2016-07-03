#include "rhine/Transform/Scope2Block.hpp"
#include "rhine/Diagnostic/Diagnostic.hpp"
#include "rhine/IR/BasicBlock.hpp"
#include "rhine/IR/Constant.hpp"
#include "rhine/IR/Context.hpp"
#include "rhine/IR/Function.hpp"
#include "rhine/IR/Instruction.hpp"

namespace rhine {
Scope2Block::Scope2Block() : K(nullptr) {}

using SetFcn = std::function<void(std::vector<BasicBlock>)>;

static void setPredParentSucc(BasicBlock *Block,
                              std::vector<BasicBlock *> PredList,
                              Function *Parent,
                              std::vector<BasicBlock *> SuccList) {
  Block->setPredecessors(PredList);
  Block->setParent(Parent);
  if (SuccList.size() && SuccList[0]) {
    Block->setSuccessors(SuccList);
  }
}

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
  auto MergeBlock =
      BasicBlock::get("exit", make_range(StartInst, Cleavee->end()), K);
  BranchInst->setNext(*Cleavee->end());
  Cleavee->end()->setPrev(BranchInst);

  /// Set up predecessors and successors.
  Cleavee->setSuccessors({TrueBlock, FalseBlock});

  setPredParentSucc(TrueBlock, {Cleavee}, Parent, {MergeBlock});
  setPredParentSucc(FalseBlock, {Cleavee}, Parent, {MergeBlock});
  setPredParentSucc(MergeBlock, {TrueBlock, FalseBlock}, Parent, {ReturnTo});

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
  InstListType::iterator It;
  for (It = BB->begin(); std::next(It) != BB->end(); ++It)
    if (isa<TerminatorInst>(*It) || isa<ReturnInst>(*It)) {
      DiagnosticPrinter(It->sourceLocation()) << "unexpected block terminator";
      exit(1);
    }
}

void Scope2Block::runOnFunction(Function *F) {
  K = F->context();
  auto EntryBlock = F->getEntryBlock();
  EntryBlock->setParent(F);
  cleaveBlockAtBranches(EntryBlock);
  for (auto BB : *F)
    validateBlockForm(BB);
}
}
