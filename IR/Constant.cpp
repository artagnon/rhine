#include "rhine/IR/Constant.h"

namespace rhine {
Constant::Constant(Type *Ty, RTValue ID) : Value(Ty, ID) {}

ConstantInt::ConstantInt(int Val, unsigned Bitwidth, Context *K) :
    Constant(IntegerType::get(Bitwidth, K), RT_ConstantInt), Val(Val) {}

ConstantInt *ConstantInt::get(int Val, unsigned Bitwidth, Context *K) {
  return new (K->RhAllocator) ConstantInt(Val, Bitwidth, K);
}

bool ConstantInt::classof(const Value *V) {
  return V->getValID() == RT_ConstantInt;
}

int ConstantInt::getVal() {
  return Val;
}

unsigned ConstantInt::getBitwidth() {
  if (auto ITy = dyn_cast<IntegerType>(VTy))
    return ITy->getBitwidth();
  assert(0 && "ConstantInt of non IntegerType type");
}

void ConstantInt::print(std::ostream &Stream) const {
  Stream << Val << " ~" << *getType();
}

ConstantBool::ConstantBool(bool Val, Context *K) :
    Constant(BoolType::get(K), RT_ConstantBool), Val(Val) {}

ConstantBool *ConstantBool::get(bool Val, Context *K) {
  return new (K->RhAllocator) ConstantBool(Val, K);
}

bool ConstantBool::classof(const Value *V) {
  return V->getValID() == RT_ConstantBool;
}

float ConstantBool::getVal() {
  return Val;
}

void ConstantBool::print(std::ostream &Stream) const {
  Stream << Val << " ~" << *getType();
}

ConstantFloat::ConstantFloat(float Val, Context *K) :
    Constant(FloatType::get(K), RT_ConstantFloat), Val(Val) {}

ConstantFloat *ConstantFloat::get(float Val, Context *K) {
  return new (K->RhAllocator) ConstantFloat(Val, K);
}

bool ConstantFloat::classof(const Value *V) {
  return V->getValID() == RT_ConstantFloat;
}

float ConstantFloat::getVal() {
  return Val;
}

void ConstantFloat::print(std::ostream &Stream) const {
  Stream << Val << " ~" << *getType();
}

Function::Function(FunctionType *FTy) :
    Value(FTy, RT_Function), ParentModule(nullptr),
    VariadicRestSymbol(nullptr), Val(nullptr) {}

Function *Function::get(FunctionType *FTy, Context *K) {
  return new (K->RhAllocator) Function(FTy);
}

bool Function::classof(const Value *V) {
  return V->getValID() == RT_Function;
}

void Function::setParent(Module *Parent) {
  ParentModule = Parent;
}

Module *Function::getParent() {
  return ParentModule;
}

void Function::setName(std::string N) {
  Name = N;
}

std::string Function::getName() {
  return Name;
}

void Function::setArguments(std::vector<Argument *> L) {
  ArgumentList = L;
}

void Function::setVariadicRest(Argument *Rest) {
  if (!Rest)
    return;
  assert(cast<FunctionType>(VTy)->isVariadic() &&
         "Confusion about whether function is variadic");
  VariadicRestSymbol = Rest;
}

std::vector<Argument *> Function::getArguments() {
  return ArgumentList;
}

void Function::setBody(BasicBlock *Body) {
  Val = Body;
}

BasicBlock *Function::getVal() {
  return Val;
}

BasicBlock::iterator Function::begin() {
  return Val->ValueList.begin();
}

BasicBlock::iterator Function::end() {
  return Val->ValueList.end();
}

void Function::print(std::ostream &Stream) const {
  Stream << Name << " ~" << *getType() << std::endl;
  for (auto A: ArgumentList)
    Stream << *A << std::endl;
  if (VariadicRestSymbol)
    Stream << "&" << *VariadicRestSymbol << std::endl;
  for (auto V: Val->ValueList)
    Stream << *V << std::endl;
}
}
