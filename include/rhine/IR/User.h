//-*- C++ -*-
#ifndef RHINE_USER_H
#define RHINE_USER_H

#include "rhine/IR/Value.h"
#include "rhine/IR/Use.h"

namespace rhine {
class User : public Value {
protected:
  unsigned NumOperands;
public:
  User(Type *Ty, RTValue ID, unsigned NumOps, std::string N);
  void *operator new(size_t Size, unsigned Us);
  static bool classof(const Value *V);
  Use *getOperandList();
  const Use *getOperandList() const;
  Value *getOperand(unsigned i) const;
  void setOperand(unsigned i, Value *Val);
};
}

#endif
