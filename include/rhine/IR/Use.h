//-*- C++ -*-
#ifndef RHINE_USE_H
#define RHINE_USE_H

namespace rhine {
class Value;
class User;

class Use {
  Value *Val;
  Use *Prev;
  Use *Next;
  unsigned OperandNumber;
public:
  unsigned getOperandNumber();
  void setOperandNumber(unsigned Num);
  void setVal(Value *Val_);
  User *getUser();
  Value *operator=(Value *RHS);
  Value *operator->();
  operator Value *() const;
  void swap(Use &RHS);
  void addToList(Use *UseList);
  void removeFromList();

  friend class Value;
};
}
#endif
