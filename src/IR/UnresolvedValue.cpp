#include "rhine/IR/UnresolvedValue.hpp"
#include "rhine/IR/Type.hpp"

namespace rhine {
UnresolvedValue::UnresolvedValue(std::string N, Type *T) :
    User(T, RT_UnresolvedValue, 0, N) {}

UnresolvedValue::~UnresolvedValue() {}

UnresolvedValue *UnresolvedValue::get(std::string N, Type *T) {
  return new UnresolvedValue(N, T);
}

bool UnresolvedValue::classof(const Value *V) {
  return V->op() == RT_UnresolvedValue;
}

void UnresolvedValue::print(DiagnosticPrinter &Stream) const {
  Stream << Name << " " << *getType();
}

Argument::Argument(std::string N, Type *T) :
    User(T, RT_Argument, 0, N) {}

Argument::~Argument() {}

Argument *Argument::get(std::string N, Type *T) {
  return new Argument(N, T);
}

bool Argument::classof(const Value *V) {
  return V->op() == RT_Argument;
}

void Argument::setParent(Prototype *P) {
  Parent = P;
}

void Argument::print(DiagnosticPrinter &Stream) const {
  Stream << Name << " " << *getType();
}
}
