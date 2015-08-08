#include "rhine/IR/Value.h"
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

void Value::addUse(Use &U) {
  U.addToList(UseList);
}

__attribute__((used, noinline))
void Value::dump() {
  std::cout << *this << std::endl;
}
}
