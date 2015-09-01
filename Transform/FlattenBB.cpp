#include "rhine/Transform/FlattenBB.h"
#include "rhine/IR/Instruction.h"
#include "rhine/IR/BasicBlock.h"
#include "rhine/IR/Constant.h"

namespace rhine {
class Value;

FlattenBB::FlattenBB() : K(nullptr) {}

FlattenBB::~FlattenBB() {}

void FlattenBB::cleaveBB(BasicBlock *Cleavee, Function *Parent) {
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
  auto NewBB = BasicBlock::get(
      std::vector<Value *>(StartInst, Cleavee->end()), K);
  Cleavee->ValueList.erase(StartInst, Cleavee->end());

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

void FlattenBB::runOnFunction(Function *F) {
  K = F->getContext();
  cleaveBB(F->getEntryBlock(), F);
}
}
