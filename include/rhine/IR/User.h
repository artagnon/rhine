//-*- C++ -*-
#ifndef RHINE_USER_H
#define RHINE_USER_H

#include "rhine/IR/Value.h"
#include "rhine/IR/Use.h"

namespace rhine {
class User : public Value {
protected:
  unsigned NumOperands;
  unsigned NumAllocatedOps;
public:
  User(Type *Ty, RTValue ID, unsigned NumOps = 0, std::string N = "");
  void *operator new(size_t Size, unsigned Us);
  void *operator new(size_t Size);
  void operator delete(void *Usr);
  static bool classof(const Value *V);
  Use *getOperandList();
  const Use *getOperandList() const;
  typedef Use* op_iterator;
  op_iterator op_begin() { return getOperandList(); }
  op_iterator op_end() { return getOperandList() + NumAllocatedOps; }
  iterator_range<op_iterator> operands() {
    return iterator_range<op_iterator>(op_begin(), op_end());
  }
  virtual Value *getOperand(int i) const;
  virtual void setOperand(int i, Value *Val);
};
}

#endif
