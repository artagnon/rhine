#ifndef RHINE_USER_H
#define RHINE_USER_H

#include "rhine/IR/Use.hpp"
#include "rhine/IR/Value.hpp"
#include <vector>

namespace rhine {
class User : public Value {
protected:
  /// In the case of CallInst, the Callee itself is operand 0, which
  /// NumAllocatedOps accounts for, and NumOperands omits. See corresponding
  /// iterators uses() and operands().
  unsigned NumOperands;
  unsigned NumAllocatedOps;

public:
  User(Type *Ty, RTValue ID, unsigned NumOps = 0, std::string N = "");

  /// Noop.
  virtual ~User();

  /// Allocate memory for Us uses too.
  void *operator new(size_t Size, unsigned Us);

  /// Allocate memory for zero uses.
  void *operator new(size_t Size);

  /// Delete the User, but don't touch the Uses.
  void operator delete(void *Usr);

  /// For dyn_cast.
  static bool classof(const Value *V);

  /// "Let go" of all objects that this User refers to.  This allows one to
  /// 'delete' a whole class at a time, even though there may be circular
  /// references...  First all references are dropped, and all use counts go to
  /// zero.  Then everything is deleted for real.  Note that no operations are
  /// valid on an object that has "dropped all references", except operator
  /// delete.
  void dropAllReferences();

  /// Raw Use pointers to the Operand list, plus its const cousin.
  Use *getOperandList();
  const Use *getOperandList() const;

  /// Get and set the ith "real" operand, omitting Callee in CallInst for
  /// instance.
  Value *getOperand(int i) const;
  void setOperand(int i, Value *Val);

  /// Just an accumultator over the iterators.
  std::vector<Value *> getOperands() const;
  void setOperands(std::vector<Value *> Ops);

  /// The iterators for Use and Operand.
  typedef Use *op_iterator;
  op_iterator use_begin() { return getOperandList(); }
  op_iterator use_end() { return getOperandList() + NumAllocatedOps; }
  op_iterator op_begin() {
    return getOperandList() + NumAllocatedOps - NumOperands;
  }
  op_iterator op_end() { return getOperandList() + NumAllocatedOps; }

  /// Go over AllocatedOps.
  iterator_range<op_iterator> uses() {
    return iterator_range<op_iterator>(use_begin(), use_end());
  }

  /// Go over only the real operands, omitting Callee in CallInst for instance.
  iterator_range<op_iterator> operands() {
    return iterator_range<op_iterator>(op_begin(), op_end());
  }
};
}

#endif
