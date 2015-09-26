#include "rhine/IR/BasicBlock.h"
#include "rhine/IR/Instruction.h"
#include "rhine/IR/Module.h"
#include "rhine/Context.h"

namespace rhine {
void BasicBlock::setAllInstructionParents(std::vector<Value *> List) {
  for (auto V : List) {
    if (auto Inst = dyn_cast<Instruction>(V)) {
      Inst->setParent(this);
      setAllInstructionParents(Inst->getOperands());
    }
  }
}

BasicBlock::BasicBlock(Type *Ty, std::string Name, std::vector<Value *> V) :
    Value(Ty, RT_BasicBlock, Name), Parent(nullptr), ValueList(V) {
  setAllInstructionParents(ValueList);
}

BasicBlock::~BasicBlock() {}

BasicBlock *BasicBlock::get(std::string Name, std::vector<Value *> V,
                            Context *K) {
  return new BasicBlock(UnType::get(K), Name, V);
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

iterator_range<BasicBlock::bb_iterator> BasicBlock::preds() {
  return iterator_range<BasicBlock::bb_iterator>(pred_begin(), pred_end());
}

BasicBlock::bb_iterator BasicBlock::succ_begin() {
  return Successors.begin();
}

BasicBlock::bb_iterator BasicBlock::succ_end() {
  return Successors.end();
}

iterator_range<BasicBlock::bb_iterator> BasicBlock::succs() {
  return iterator_range<BasicBlock::bb_iterator>(succ_begin(), succ_end());
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
  if (Predecessors.size() == 1)
    return Predecessors[0];
  return nullptr;
}

BasicBlock *BasicBlock::getUniqueSuccessor() const {
  if (Successors.size() == 1)
    return Successors[0];
  return nullptr;
}

void BasicBlock::print(std::ostream &Stream) const {
  for (auto V: ValueList)
    Stream << *V << std::endl;
}
}
