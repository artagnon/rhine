#include "rhine/Transform/Scope2Block.h"
#include "rhine/IR/Instruction.h"
#include "rhine/IR/BasicBlock.h"
#include "rhine/IR/Constant.h"

namespace rhine {
class Value;

Scope2Block::Scope2Block() : K(nullptr) {}

Scope2Block::~Scope2Block() {}

void Scope2Block::cleaveBB(BasicBlock *Cleavee, Function *Parent) {
  auto It = std::find_if(Cleavee->begin(), Cleavee->end(),
                         [](Value *Arg) {
                           return isa<IfInst>(Arg);
                         });
  if (It == Cleavee->end())
    return;
  auto BranchInst = cast<IfInst>(*It);
  auto TrueBB = BranchInst->getTrueBB();
  auto FalseBB = BranchInst->getFalseBB();
  auto StartInst = std::next(It);
  auto NewBB = BasicBlock::get("exit",
      std::vector<Instruction *>(StartInst, Cleavee->end()), K);
  Cleavee->InstList.erase(StartInst, Cleavee->end());

  // Set up predecessors and successors
  Cleavee->addSuccessors({TrueBB, FalseBB});
  Cleavee->setParent(Parent);
  TrueBB->addPredecessors({Cleavee});
  TrueBB->setParent(Parent);
  FalseBB->addPredecessors({Cleavee});
  FalseBB->setParent(Parent);
  TrueBB->addSuccessors({NewBB});
  FalseBB->addSuccessors({NewBB});
  NewBB->addPredecessors({TrueBB, FalseBB});
  NewBB->setParent(Parent);

  auto ParentFunction = Cleavee->getParent();
  ParentFunction->push_back(TrueBB);
  ParentFunction->push_back(FalseBB);
  ParentFunction->push_back(NewBB);
  return cleaveBB(NewBB, Parent);
}

void Scope2Block::runOnFunction(Function *F) {
  K = F->getContext();
  auto EntryBlock = F->getEntryBlock();
  EntryBlock->setParent(F);
  cleaveBB(EntryBlock, F);
}
}
