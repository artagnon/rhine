//-*- C++ -*-

#ifndef RHINE_BASICBLOCK_H
#define RHINE_BASICBLOCK_H

#include "rhine/IR/Value.h"

namespace rhine {
class BasicBlock : public Value {
  Function *Parent;
public:
  std::vector<Value *> ValueList;
  BasicBlock(Type *Ty, std::vector<Value *> V);
  static BasicBlock *get(std::vector<Value *> V, Context *K);
  static bool classof(const Value *V);
  llvm::Value *toLL(llvm::Module *M);
  typedef std::vector<Value *>::iterator iterator;
  iterator begin();
  iterator end();
  unsigned size();
  Value *back();
  Function *getParent() const;
protected:
  virtual void print(std::ostream &Stream) const;
};
}

#endif
