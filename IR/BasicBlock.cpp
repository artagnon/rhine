#include "rhine/IR/BasicBlock.h"
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

BasicBlock::iterator BasicBlock::begin() {
  return ValueList.begin();
}

BasicBlock::iterator BasicBlock::end() {
  return ValueList.end();
}

unsigned BasicBlock::size() {
  return ValueList.size();
}

Value *BasicBlock::back() {
  return ValueList.back();
}

Function *BasicBlock::getParent() const {
  return Parent;
}

void BasicBlock::print(std::ostream &Stream) const {
  for (auto V: ValueList)
    Stream << *V << std::endl;
}
}
