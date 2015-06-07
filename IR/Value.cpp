#include "rhine/IR/Value.h"
#include "rhine/IR/Constant.h"
#include <iostream>

namespace rhine {

Value::Value(Type *VTy, RTValue ID) : VTy(VTy), ValID(ID) {}

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

__attribute__((used, noinline))
void Value::dump() {
  std::cout << *this << std::endl;
}

Symbol::Symbol(std::string N, Type *T) : Value(T, RT_Symbol), Name(N) {}

Symbol *Symbol::get(std::string N, Type *T, Context *K) {
  return new (K->RhAllocator) Symbol(N, T);
}

bool Symbol::classof(const Value *V) {
  return V->getValID() == RT_Symbol;
}

std::string Symbol::getName() {
  return Name;
}

void Symbol::print(std::ostream &Stream) const {
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
  Stream << "\"" << Val << "\" ~" << *getType();
}
}
