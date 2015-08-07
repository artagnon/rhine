#include "rhine/IR/User.h"
#include "rhine/IR/Constant.h"

namespace rhine {
void *User::operator new(size_t Size, unsigned Us) {
  void *Storage = ::operator new (Us * sizeof(Use) + Size);
  auto Start = static_cast<Use *>(Storage);
  auto End = Start + Us;
  for (unsigned Iter = 0; Iter < Us; Iter++) {
    auto Obj = Start + Iter;
    Obj->setOperandNumber(Iter);
  }
  auto Obj = reinterpret_cast<User *>(End);
  Obj->NumOperands = Us;
  return Obj;
}

bool User::classof(const Value *V) {
  return V->getValID() == RT_User;
}

Use *User::getOperandList() {
  return reinterpret_cast<Use *>(this) - NumOperands;
}

void User::setOperand(unsigned i, Value *Val) {
  assert(i < NumOperands && "setOperand() out of range!");
  assert(!isa<Constant>(cast<Value>(this)) &&
         "Cannot mutate a constant with setOperand!");
  getOperandList()[i] = Val;
}
}
