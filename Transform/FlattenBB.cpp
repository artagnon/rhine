#include "rhine/Transform/FlattenBB.h"
#include "rhine/IR/Instruction.h"
#include "rhine/IR/BasicBlock.h"
#include "rhine/IR/Constant.h"

namespace rhine {
class Value;

FlattenBB::FlattenBB() : K(nullptr) {}

FlattenBB::~FlattenBB() {}

void FlattenBB::cleaveBB(BasicBlock *Cleavee) {
  auto It = std::find_if(Cleavee->begin(), Cleavee->end(),
                         [](Value *Arg) {
                           return isa<IfInst>(Arg);
                         });
  if (It == Cleavee->end())
    return;
  auto Inst = cast<IfInst>(*It);
  auto TrueBB = Inst->getTrueBB();
  auto FalseBB = Inst->getFalseBB();
  auto Start = std::next(It);
  auto New = BasicBlock::get(std::vector<Value *>(Start, Cleavee->end()), K);
  Cleavee->ValueList.erase(Start, Cleavee->end());

  // Set up predecessors and successors
  Cleavee->addSuccessors({TrueBB, FalseBB});
  TrueBB->addPredecessors({Cleavee});
  FalseBB->addPredecessors({Cleavee});
  TrueBB->addSuccessors({New});
  FalseBB->addSuccessors({New});
  New->addPredecessors({TrueBB, FalseBB});

  auto ParentFunction = Cleavee->getParent();
  ParentFunction->push_back(TrueBB);
  ParentFunction->push_back(FalseBB);
  ParentFunction->push_back(New);
  return cleaveBB(New);
}

void FlattenBB::runOnFunction(Function *F) {
  K = F->getContext();
  cleaveBB(F->getEntryBlock());
}
}
