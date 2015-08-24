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
  Value *getOperand(int i) const;
  void setOperand(int i, Value *Val);
  std::vector<Value *> getOperands() const;
  void setOperands(std::vector<Value *> Ops);
  typedef Use* op_iterator;
  op_iterator use_begin() { return getOperandList(); }
  op_iterator use_end() { return getOperandList() + NumAllocatedOps; }
  op_iterator op_begin() {
    return getOperandList() + NumAllocatedOps - NumOperands;
  }
  op_iterator op_end() { return getOperandList() + NumAllocatedOps; }
  iterator_range<op_iterator> uses() {
    return iterator_range<op_iterator>(use_begin(), use_end());
  }
  iterator_range<op_iterator> operands() {
    return iterator_range<op_iterator>(op_begin(), op_end());
  }
};
}

#endif
