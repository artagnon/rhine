#include "rhine/IR/BasicBlock.h"
#include "rhine/IR/Module.h"
#include "rhine/Context.h"

namespace rhine {
BasicBlock::BasicBlock(Type *Ty, std::vector<Value *> V) :
    Value(Ty, RT_BasicBlock), Parent(nullptr), ValueList(V) {}

BasicBlock::~BasicBlock() {}

BasicBlock *BasicBlock::get(std::vector<Value *> V, Context *K) {
  return new BasicBlock(UnType::get(K), V);
}

bool BasicBlock::classof(const Value *V) {
  return V->getValID() == RT_BasicBlock;
}

BasicBlock::value_iterator BasicBlock::begin() {
  return ValueList.begin();
}

BasicBlock::value_iterator BasicBlock::end() {
  return ValueList.end();
}

BasicBlock::bb_iterator BasicBlock::pred_begin() {
  return Predecessors.begin();
}

BasicBlock::bb_iterator BasicBlock::pred_end() {
  return Predecessors.end();
}

BasicBlock::bb_iterator BasicBlock::succ_begin() {
  return Successors.begin();
}

BasicBlock::bb_iterator BasicBlock::succ_end() {
  return Successors.end();
}

void BasicBlock::addPredecessors(std::vector<BasicBlock *> Preds) {
  for (auto B: Preds)
    Predecessors.push_back(B);
}

void BasicBlock::removePredecessor(BasicBlock *Pred) {
  auto It = std::remove(Predecessors.begin(), Predecessors.end(), Pred);
  assert(It != Predecessors.end() && "Can't find predecessor to remove");
  Predecessors.erase(It, Predecessors.end());
}

void BasicBlock::addSuccessors(std::vector<BasicBlock *> Succs) {
  for (auto B: Succs)
    Successors.push_back(B);
}

void BasicBlock::removeSuccessor(BasicBlock *Succ) {
  auto It = std::remove(Successors.begin(), Successors.end(), Succ);
  assert(It != Predecessors.end() && "Can't find successor to remove");
  Successors.erase(It, Successors.end());
}

unsigned BasicBlock::size() {
  return ValueList.size();
}

Value *BasicBlock::back() {
  return ValueList.back();
}

void BasicBlock::setParent(Function *F) {
  Parent = F;
}

Function *BasicBlock::getParent() const {
  return Parent;
}

BasicBlock *BasicBlock::getUniquePredecessor() const {
  return nullptr;
}

void BasicBlock::print(std::ostream &Stream) const {
  for (auto V: ValueList)
    Stream << *V << std::endl;
}
}
