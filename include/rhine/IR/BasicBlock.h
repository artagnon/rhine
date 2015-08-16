//-*- C++ -*-

#ifndef RHINE_BASICBLOCK_H
#define RHINE_BASICBLOCK_H

#include "rhine/IR/Value.h"

namespace rhine {
class Module;

class BasicBlock : public Value {
  Module *Parent;
public:
  std::vector<Value *> ValueList;
  BasicBlock(Type *Ty, std::vector<Value *> V);
  virtual ~BasicBlock();
  static BasicBlock *get(std::vector<Value *> V, Context *K);
  static bool classof(const Value *V);
  llvm::Value *toLL(llvm::Module *M);
  typedef std::vector<Value *>::iterator iterator;
  iterator begin();
  iterator end();
  unsigned size();
  Value *back();
  void setParent(Module *M);
  Module *getParent() const;
protected:
  virtual void print(std::ostream &Stream) const;
};
}

#endif
