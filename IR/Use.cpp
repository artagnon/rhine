#include "rhine/IR/Use.h"

namespace rhine {
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
}
