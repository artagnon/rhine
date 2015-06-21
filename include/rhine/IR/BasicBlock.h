//-*- C++ -*-

#ifndef RHINE_BASICBLOCK_H
#define RHINE_BASICBLOCK_H

#include "rhine/IR/Value.h"

namespace rhine {
class BasicBlock {
  std::vector<Value *> ValueList;
public:
  BasicBlock(std::vector<Value *> V) : ValueList(V) {}
  static BasicBlock *get(std::vector<Value *> V, Context *K) {
    return new (K->RhAllocator) BasicBlock(V);
  }
};
}

#endif
