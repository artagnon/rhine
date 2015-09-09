#include "rhine/IR/Value.h"
#include "rhine/IR/Type.h"
#include "rhine/IR/Use.h"
#include <iostream>

using Location = rhine::Parser::Location;

namespace rhine {
Value::Value(Type *VTy, RTValue ID, std::string N) :
    VTy(VTy), UseList(nullptr), Name(N), ValID(ID) {
  static std::string DummyStr = "";
  SourceLoc.Begin.Line = SourceLoc.End.Line = 0;
  SourceLoc.Begin.Column = SourceLoc.End.Column = 0;
  SourceLoc.Begin.Filename = SourceLoc.End.Filename = &DummyStr;
}

bool Value::classof(const Value *V) {
  return V->getValID() >= RT_UnresolvedValue &&
    V->getValID() <= RT_BasicBlock;
}

Context *Value::getContext() { return VTy->getContext(); }

void Value::setSourceLocation(Location SrcLoc) {
  SourceLoc = SrcLoc;
}

Location Value::getSourceLocation() {
  return SourceLoc;
}

RTValue Value::getValID() const { return ValID; }

Type *Value::getType() const {
  return VTy;
}

void Value::setType(Type *T) {
  VTy = T;
}

bool Value::isUnTyped() {
  return VTy == UnType::get(getContext());
}

std::string Value::getName() const {
  return Name;
}

void Value::setName(std::string Str) {
  Name = Str;
}

Value::operator Use *() const {
  return UseList;
}

User *Value::getUser() const {
  if (!UseList) return nullptr;
  return UseList->getUser();
}

void Value::addUse(Use &U) {
  U.addToList(UseList);
}

bool Value::use_empty() const { return UseList == nullptr; }

void Value::replaceAllUsesWith(Value *New) {
  assert(New && "Value::replaceAllUsesWith(<null>) is invalid!");

  while (!use_empty()) {
    Use &U = *UseList;
    U.set(New);
  }
}

void Value::zapUseList() { UseList = nullptr; }

__attribute__((used, noinline))
void Value::dump() {
  std::cout << *this << std::endl;
}
}
