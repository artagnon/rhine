#include "rhine/IR/User.hpp"
#include "rhine/IR/Constant.hpp"

namespace rhine {
User::User(Type *Ty, RTValue ID, unsigned NumOps, std::string N) :
    Value(Ty, ID, N), NumOperands(NumOps), NumAllocatedOps(NumOps) {}

User::~User() {}

void *User::operator new(size_t Size, unsigned Us) {
  void *Storage = ::operator new (Us * sizeof(Use) + Size);
  auto Start = static_cast<Use *>(Storage);
  auto End = Start + Us;
  for (unsigned Iter = 0; Iter < Us; Iter++) {
    new (Start + Iter) Use(Us - Iter);
  }
  auto Obj = reinterpret_cast<User *>(End);
  return Obj;
}

void *User::operator new(size_t Size) {
  return ::operator new (Size);
}

void User::operator delete(void *Usr) {
  User *Obj = static_cast<User *>(Usr);
  Use *Storage = static_cast<Use *>(Usr) - Obj->NumAllocatedOps;
  Use::zap(Storage, Storage + Obj->NumAllocatedOps, /* Delete */ false);
  ::operator delete(Storage);
}

void User::dropAllReferences() {
  for (auto &U : uses())
    U.set(nullptr);
}

bool User::classof(const Value *V) {
  return V->op() >= RT_User &&
    V->op() <= RT_IfInst;
}

Use *User::getOperandList() {
  return reinterpret_cast<Use *>(this) - NumAllocatedOps;
}

const Use *User::getOperandList() const {
  return const_cast<User *>(this)->getOperandList();
}

Value *User::getOperand(int i) const {
  int Offset = NumAllocatedOps - NumOperands;
  assert(i >= -Offset && "getOperand() given negative value!");
  assert(i < (int)NumOperands && "getOperand() out of range!");
  return getOperandList()[i + Offset];
}

void User::setOperand(int i, Value *Val) {
  int Offset = NumAllocatedOps - NumOperands;
  assert(i >= -Offset && "setOperand() given negative value!");
  assert(i < (int)NumOperands && "setOperand() out of range!");
  assert(!isa<Constant>(cast<Value>(this)) &&
         "Cannot mutate a constant with setOperand!");
  getOperandList()[i + Offset].set(Val);
}

std::vector<Value *> User::getOperands() const {
  std::vector<Value *> OpV;
  for (unsigned OpN = 0; OpN < NumOperands; OpN++) {
    OpV.push_back(getOperand(OpN));
  }
  return OpV;
}

void User::setOperands(std::vector<Value *> Ops) {
  assert(Ops.size() == NumOperands && "Incorrect number passed to setOperands()");
  for (unsigned OpN = 0; OpN < NumOperands; OpN++) {
    setOperand(OpN, Ops[OpN]);
  }
}
}
