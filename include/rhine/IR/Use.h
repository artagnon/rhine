//-*- C++ -*-
#ifndef RHINE_USE_H
#define RHINE_USE_H

#include "rhine/IR/Value.h"

namespace rhine {
class Use {
  Value *Val;
  unsigned OperandNumber;
public:
  Use() = delete;
  void setOperandNumber(unsigned Num);
  void setVal(Value *Val_);
  class User *getUser();
  Value *operator=(Value *RHS);
  Value *operator->();
  operator Value *() const;
};
}
#endif
