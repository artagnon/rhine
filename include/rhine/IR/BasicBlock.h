//-*- C++ -*-

#ifndef RHINE_BASICBLOCK_H
#define RHINE_BASICBLOCK_H

#include "rhine/IR/Value.h"

namespace rhine {
class BasicBlock : public Value {
public:
  std::string Name;
  std::vector<Value *> ValueList;
  BasicBlock(Type *Ty, std::vector<Value *> V, std::string Name) :
      Value(Ty, RT_BasicBlock), ValueList(V) {}
  static BasicBlock *get(std::vector<Value *> V, Context *K) {
    return new (K->RhAllocator) BasicBlock(UnType::get(K), V, "");
  }
  static BasicBlock *get(std::vector<Value *> V, std::string Name, Context *K) {
    return new (K->RhAllocator) BasicBlock(UnType::get(K), V, Name);
  }
  static bool classof(const Value *V) {
    return V->getValID() == RT_BasicBlock;
  }
  friend ostream &operator<<(ostream &Stream, const BasicBlock &B) {
    B.print(Stream);
    return Stream;
  }
  llvm::Value *toLL(llvm::Module *M, Context *K);
  typedef std::vector<Value *>::iterator iterator;
  iterator begin() {
    return ValueList.begin();
  }
  iterator end() {
    return ValueList.end();
  }
  unsigned size() {
    return ValueList.size();
  }
  Value *back() {
    return ValueList.back();
  }
protected:
  virtual void print(std::ostream &Stream) const {
    for (auto V: ValueList)
      Stream << *V << std::endl;
  }
};
}

#endif
