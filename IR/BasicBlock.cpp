#include "rhine/IR/BasicBlock.h"

namespace rhine {
BasicBlock::BasicBlock(Type *Ty, std::vector<Value *> V) :
    Value(Ty, RT_BasicBlock), ValueList(V) {}
BasicBlock *BasicBlock::get(std::vector<Value *> V, Context *K) {
  return new (K->RhAllocator) BasicBlock(UnType::get(K), V);
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

void BasicBlock::print(std::ostream &Stream) const {
  for (auto V: ValueList)
    Stream << *V << std::endl;
}
}
