#include "rhine/IR/Value.hpp"
#include "rhine/IR/Type.hpp"
#include "rhine/IR/Use.hpp"
#include <iostream>

using Location = rhine::Parser::Location;

namespace rhine {
Value::Value(Type *VTy, RTValue ID, std::string N)
    : VTy(VTy), UseList(nullptr), Name(N), LoweredValue(nullptr), ValID(ID) {}

Value::~Value() {}

bool Value::classof(const Value *V) {
  return V->op() >= RT_UnresolvedValue && V->op() <= RT_BasicBlock;
}

Context *Value::context() { return VTy->context(); }

void Value::setSourceLocation(Location SrcLoc) { SourceLoc = SrcLoc; }

Location Value::sourceLocation() { return SourceLoc; }

RTValue Value::op() const { return ValID; }

Type *Value::getType() const { return VTy; }

Type *Value::getRTy() const { return VTy; }

llvm::Value *Value::getLoweredValue() const { return LoweredValue; }

void Value::setLoweredValue(llvm::Value *V) { LoweredValue = V; }

void Value::setType(Type *T) { VTy = T; }

bool Value::isUnTyped() { return VTy->getTyID() == RT_UnType; }

std::string Value::getName() const { return Name; }

void Value::setName(std::string Str) { Name = Str; }

Value::operator Use *() const { return UseList; }

User *Value::getUser() const {
  if (!UseList)
    return nullptr;
  return UseList->getUser();
}

void Value::addUse(Use &U) { U.addToList(UseList); }

bool Value::use_empty() const { return UseList == nullptr; }

void Value::replaceAllUsesWith(Value *New) {
  assert(New && "Value::replaceAllUsesWith(<null>) is invalid!");

  while (!use_empty()) {
    Use &U = *UseList;
    U.set(New);
  }
}

void Value::zapUseList() { UseList = nullptr; }

__attribute__((used, noinline)) void Value::dump() {
  std::cout << *this << std::endl;
}
}
