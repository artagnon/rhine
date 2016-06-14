#include "rhine/IR/Use.hpp"
#include "rhine/IR/User.hpp"
#include "rhine/IR/Value.hpp"

namespace rhine {
Use::Use(unsigned Dist) :
    Val(nullptr), Prev(nullptr),
    Next(nullptr), DistToUser(Dist) {}

Use::~Use() {}

unsigned Use::getOperandNumber() {
  return DistToUser;
}

void Use::setOperandNumber(unsigned Num) {
  DistToUser = Num;
}

void Use::setVal(Value *V) {
  Val = V;
}

class User *Use::getUser() {
  return reinterpret_cast<User *>(this + DistToUser);
}

Value *Use::val() const {
  return Val;
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

void Use::addToList(Use *&UseList) {
  if (!UseList) {
    UseList = this;
    return;
  }
  UseList->Next = this;
  Prev = UseList;
  UseList = this;
}

void Use::removeFromList() {
  if (!Prev && !Next)
    Val->zapUseList();
  if (Prev)
    Prev->Next = Next;
  if (Next)
    Next->Prev = Prev;
}

void Use::set(Value *V) {
  if (Val) removeFromList();
  Val = V;
  if (V) V->addUse(*this);
}

void Use::zap(Use *Start, const Use *Stop, bool Del) {
  while (Start != Stop)
    (--Stop)->~Use();
  if (Del)
    ::operator delete(Start);
}
}
