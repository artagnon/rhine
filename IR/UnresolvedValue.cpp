#include "rhine/IR/UnresolvedValue.h"

namespace rhine {
UnresolvedValue::UnresolvedValue(std::string N, Type *T, RTValue ValID) :
    User(T, ValID, 0, N) {}

UnresolvedValue *UnresolvedValue::get(std::string N, Type *T, Context *K) {
  return new UnresolvedValue(N, T);
}

bool UnresolvedValue::classof(const Value *V) {
  return V->getValID() >= RT_UnresolvedValue &&
    V->getValID() <= RT_Argument;
}

void UnresolvedValue::print(std::ostream &Stream) const {
  Stream << Name << " ~" << *getType();
}

Argument::Argument(std::string N, Type *T) :
    UnresolvedValue(N, T, RT_Argument) {}

Argument *Argument::get(std::string N, Type *T, Context *K) {
  return new Argument(N, T);
}

bool Argument::classof(const Value *V) {
  return V->getValID() == RT_Argument;
}

void Argument::print(std::ostream &Stream) const {
  Stream << Name << " ~" << *getType();
}
}
