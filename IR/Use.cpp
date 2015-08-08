#include "rhine/IR/Use.h"
#include "rhine/IR/User.h"
#include "rhine/IR/Value.h"

namespace rhine {
Use::Use() : Val(nullptr), Prev(nullptr), Next(nullptr), OperandNumber(0) {}

unsigned Use::getOperandNumber() {
  return OperandNumber;
}

void Use::setOperandNumber(unsigned Num) {
  OperandNumber = Num;
}

void Use::setVal(Value *V) {
  Val = V;
}

class User *Use::getUser() {
  return reinterpret_cast<User *>(this - OperandNumber);
}

Value *Use::operator=(Value *RHS) {
  setVal(RHS);
  return RHS;
}

Value *Use::operator->() { return Val; }

Use::operator Value *() const { return Val; }

void Use::swap(Use &RHS) {
  if (Val == RHS.Val)
    return;

  if (Val)
    removeFromList();

  Value *OldVal = Val;
  if (RHS.Val) {
    RHS.removeFromList();
    Val = RHS.Val;
    Val->addUse(*this);
  } else {
    Val = nullptr;
  }

  if (OldVal) {
    RHS.Val = OldVal;
    RHS.Val->addUse(RHS);
  } else {
    RHS.Val = nullptr;
  }
}

void Use::addToList(Use *UseList) {
  if (!UseList) {
    Prev = nullptr;
    return;
  }
  UseList->Next = this;
  Prev = UseList;
  UseList = this;
}

void Use::removeFromList() {
  assert(Next && "Cannot remove primary Use");
  Next->Prev = nullptr;
}

void Use::set(Value *V) {
  if (Val) removeFromList();
  Val = V;
  if (V) V->addUse(*this);
}
}
