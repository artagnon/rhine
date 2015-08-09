#include "rhine/IR/Value.h"
#include <iostream>

namespace rhine {
Value::Value(Type *VTy, RTValue ID, std::string N) :
    VTy(VTy), UseList(nullptr), Name(N), ValID(ID) {}

Context *Value::getContext() { return VTy->getContext(); }

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

bool Value::use_empty() const { return UseList == nullptr; }

void Value::addUse(Use &U) {
  U.addToList(UseList);
}

void Value::replaceAllUsesWith(Value *New) {
  assert(New && "Value::replaceAllUsesWith(<null>) is invalid!");

  while (!use_empty()) {
    Use &U = *UseList;
    U.set(New);
  }
}

__attribute__((used, noinline))
void Value::dump() {
  std::cout << *this << std::endl;
}
}
