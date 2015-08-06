#include "rhine/IR/Value.h"
#include "rhine/IR/Constant.h"
#include <iostream>

namespace rhine {

Value::Value(Type *VTy, RTValue ID, std::string N) :
    VTy(VTy), Name(N), ValID(ID) {}

void Value::setSourceLocation(location SrcLoc) {
  SourceLoc = SrcLoc;
}

location Value::getSourceLocation() {
  return SourceLoc;
}

RTValue Value::getValID() const { return ValID; }

Type *Value::getType() const {
  return VTy;
}

void Value::setType(Type *T) {
  VTy = T;
}

std::string Value::getName() const {
  return Name;
}

void Value::setName(std::string Str) {
  Name = Str;
}

__attribute__((used, noinline))
void Value::dump() {
  std::cout << *this << std::endl;
}

UnresolvedValue::UnresolvedValue(std::string N, Type *T, RTValue ValID) :
    Value(T, ValID, N) {}

UnresolvedValue *UnresolvedValue::get(std::string N, Type *T, Context *K) {
  return new (K->RhAllocator) UnresolvedValue(N, T);
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
  return new (K->RhAllocator) Argument(N, T);
}

bool Argument::classof(const Value *V) {
  return V->getValID() == RT_Argument;
}

void Argument::print(std::ostream &Stream) const {
  Stream << Name << " ~" << *getType();
}

GlobalString::GlobalString(std::string Val, Context *K) :
    Value(StringType::get(K), RT_GlobalString), Val(Val) {}

GlobalString *GlobalString::get(std::string Val, Context *K) {
  return new (K->RhAllocator) GlobalString(Val, K);
}

bool GlobalString::classof(const Value *V) {
  return V->getValID() == RT_GlobalString;
}

std::string GlobalString::getVal() {
  return Val;
}

void GlobalString::print(std::ostream &Stream) const {
  Stream << "'" << Val << "' ~" << *getType();
}
}
