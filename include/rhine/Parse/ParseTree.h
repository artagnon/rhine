//-*- C++ -*-

#ifndef PARSETREE_H
#define PARSETREE_H

#include "rhine/IR.h"
#include <vector>

using namespace std;

namespace rhine {
class PTree {
public:
  Module M;
};

class BasicBlockSpear {
  Value *Head;
  std::vector<BasicBlock *> BBList;
public:
  BasicBlockSpear(Value *H, std::vector<BasicBlock *> B): Head(H), BBList(B) {}
  static BasicBlockSpear *get(
      Value *H, std::vector<BasicBlock *> B, Context *K) {
    return new (K->RhAllocator) BasicBlockSpear(H, B);
  }
  Value *getHead() {
    return Head;
  }
  BasicBlock *getBB(unsigned Idx) {
    assert(Idx < BBList.size() && "Idx out of BBSpear getBB() bounds");
    return BBList[Idx];
  }
  std::vector<BasicBlock *> *getBBs() {
    return &BBList;
  }
};
}

#endif
