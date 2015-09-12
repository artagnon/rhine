#include "rhine/IR/UnresolvedValue.h"
#include "rhine/IR/Type.h"

namespace rhine {
UnresolvedValue::UnresolvedValue(std::string N, Type *T) :
    User(T, RT_UnresolvedValue, 0, N) {}

UnresolvedValue *UnresolvedValue::get(std::string N, Type *T) {
  return new UnresolvedValue(N, T);
}

bool UnresolvedValue::classof(const Value *V) {
  return V->getValID() == RT_UnresolvedValue;
}

void UnresolvedValue::print(std::ostream &Stream) const {
  Stream << Name << " ~" << *getType();
}

Argument::Argument(std::string N, Type *T) :
    User(T, RT_Argument, 0, N) {}

Argument *Argument::get(std::string N, Type *T) {
  return new Argument(N, T);
}

bool Argument::classof(const Value *V) {
  return V->getValID() == RT_Argument;
}

void Argument::print(std::ostream &Stream) const {
  Stream << Name << " ~" << *getType();
}
}
