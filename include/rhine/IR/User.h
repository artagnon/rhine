//-*- C++ -*-
#ifndef RHINE_USER_H
#define RHINE_USER_H

#include "rhine/IR/Value.h"
#include "rhine/IR/Use.h"

namespace rhine {
class User : public Value {
  unsigned NumOperands;
public:
  User() = delete;
  void *operator new(size_t Size, unsigned Us);
  static bool classof(const Value *V);
  Use *getOperandList();
  void setOperand(unsigned i, Value *Val);
};
}

#endif
