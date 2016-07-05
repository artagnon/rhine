#include "rhine/IR/BasicBlock.hpp"
#include "rhine/ADT/iterator_range.hpp"
#include "rhine/IR/Context.hpp"
#include "rhine/IR/Instruction.hpp"
#include "rhine/IR/Module.hpp"

namespace rhine {

BasicBlock::BasicBlock(Type *Ty, std::string N,
                       iterator_range<inst_iterator> InstRange)
    : Value(Ty, RT_BasicBlock, N), Parent(nullptr) {
  // While appending, we change the next() of the member we're inserting.
  std::vector<Instruction *> TheList;
  for (auto I : InstRange) {
    I->setParent(this);
    TheList.push_back(I);
  }
  for (auto I : TheList) {
    InstList.append(I);
  }
}

BasicBlock::BasicBlock(Type *Ty, std::string N,
                       std::vector<Instruction *> InstRange)
    : Value(Ty, RT_BasicBlock, N), Parent(nullptr) {
  for (auto I : InstRange) {
    I->setParent(this);
    InstList.append(I);
  }
}

BasicBlock::~BasicBlock() {
  dropAllReferences();
  for (auto V : *this)
    delete V;
  InstList.clear();
}

void BasicBlock::dropAllReferences() {
  for (auto V : *this)
    V->dropAllReferences();
}

template <typename Iterable>
BasicBlock *BasicBlock::get(std::string Name, Iterable InstRange, Context *K) {
  return new BasicBlock(UnType::get(K), Name, InstRange);
}

/// Two instantiations
template BasicBlock *BasicBlock::get(std::string, iterator_range<inst_iterator>,
                                     Context *);
template BasicBlock *BasicBlock::get(std::string, std::vector<Instruction *>,
                                     Context *);

bool BasicBlock::classof(const Value *V) { return V->op() == RT_BasicBlock; }

InstListType BasicBlock::getInstList() { return InstList; }

BasicBlock::bb_iterator BasicBlock::pred_begin() {
  return Predecessors.begin();
}

BasicBlock::bb_iterator BasicBlock::pred_end() { return Predecessors.end(); }

iterator_range<BasicBlock::bb_iterator> BasicBlock::preds() {
  return iterator_range<BasicBlock::bb_iterator>(pred_begin(), pred_end());
}

BasicBlock::bb_iterator BasicBlock::succ_begin() { return Successors.begin(); }

BasicBlock::bb_iterator BasicBlock::succ_end() { return Successors.end(); }

iterator_range<BasicBlock::bb_iterator> BasicBlock::succs() {
  return iterator_range<BasicBlock::bb_iterator>(succ_begin(), succ_end());
}

void BasicBlock::addPredecessors(std::vector<BasicBlock *> Preds) {
  for (auto &B : Preds)
    Predecessors.push_back(B);
}

void BasicBlock::setPredecessors(std::vector<BasicBlock *> Preds) {
  Predecessors = Preds;
}

void BasicBlock::removePredecessor(BasicBlock *Pred) {
  auto It = std::remove(Predecessors.begin(), Predecessors.end(), Pred);
  assert(It != Predecessors.end() && "Can't find predecessor to remove");
  Predecessors.erase(It, Predecessors.end());
}

void BasicBlock::addSuccessors(std::vector<BasicBlock *> Succs) {
  for (auto &B : Succs)
    Successors.push_back(B);
}

void BasicBlock::setSuccessors(std::vector<BasicBlock *> Succs) {
  Successors = Succs;
}

void BasicBlock::removeSuccessor(BasicBlock *Succ) {
  auto It = std::remove(Successors.begin(), Successors.end(), Succ);
  assert(It != Predecessors.end() && "Can't find successor to remove");
  Successors.erase(It, Successors.end());
}

unsigned BasicBlock::size() { return InstList.size(); }

Instruction *BasicBlock::back() {
  if (!size())
    return nullptr;
  return InstList.back();
}

void BasicBlock::setParent(Function *F) { Parent = F; }

Function *BasicBlock::getParent() const { return Parent; }

bool BasicBlock::hasNoPredecessors() const { return !Predecessors.size(); }

bool BasicBlock::hasNoSuccessors() const { return !Successors.size(); }

unsigned BasicBlock::pred_size() const { return Predecessors.size(); }

unsigned BasicBlock::succ_size() const { return Successors.size(); }

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

BasicBlock *BasicBlock::getMergeBlock() {
  if (Successors.size() < 2)
    return nullptr;
  std::vector<unsigned> SuccessorTally;
  SuccessorTally.push_back(Successors.size());
  auto DigThroughBlock = this;
  while (SuccessorTally.size()) {
    DigThroughBlock = *DigThroughBlock->succ_begin();
    auto SuccSize = DigThroughBlock->succ_size();
    auto PredSize = DigThroughBlock->pred_size();
    if (SuccSize > 1)
      SuccessorTally.push_back(SuccSize);
    else if (PredSize > 1) {
      assert(SuccessorTally.back() == PredSize && "Malformed BasicBlock");
      SuccessorTally.pop_back();
    }
  }
  return DigThroughBlock;
}

void BasicBlock::print(DiagnosticPrinter &Stream) const {
  for (auto V : *this)
    Stream << *V << std::endl;
}
}
