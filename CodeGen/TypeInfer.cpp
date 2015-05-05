#include "rhine/IR.h"
#include <iostream>

namespace rhine {
//===--------------------------------------------------------------------===//
// Type inference core.
//===--------------------------------------------------------------------===//
Type *Symbol::typeInfer(Context *K) {
  Type *Ty = getType();
  if (Ty == UnType::get(K)) {
    Ty = K->getNameTypeMapping(getName());
    assert (Ty != UnType::get(K) && "Missing seed to infer type");
  }
  K->addNameTypeMapping(getName(), Ty);
  return Ty;
}

Type *ConstantInt::typeInfer(Context *K) {
  return this->getType();
}

Type *ConstantBool::typeInfer(Context *K) {
  return this->getType();
}

Type *ConstantFloat::typeInfer(Context *K) {
  return this->getType();
}

Type *GlobalString::typeInfer(Context *K) {
  return this->getType();
}

Type *Function::typeInfer(Context *K) {
  // Argument list transform
  auto V = getArgumentList();
  std::transform(V.begin(), V.end(), V.begin(),
                 [K](Symbol *S) -> Symbol * {
                   S->setType(S->typeInfer(K));
                   return S;
                 });
  // FunctionType and Body transform
  auto FType = dyn_cast<FunctionType>(getType());
  auto Body = getVal();
  Body->setType(Body->typeInfer(K));
  FType->setRTy(Body->getType());
  return FType;
}

Type *AddInst::typeInfer(Context *K) {
  auto LType = getOperand(0)->typeInfer();
  if (getOperand(1)->typeInfer() != LType)
    assert(0 && "AddInst with operands of different types");
  return getType();
}

Type *CallInst::typeInfer(Context *K) {
  return getType();
}

Type *BindInst::typeInfer(Context *K) {
  return nullptr; // Void
}

void Module::typeInfer(Context *K) {
  auto V = getVal();
  std::transform(V.begin(), V.end(), V.begin(),
                 [K](Function *F) -> Function * {
                   F->setType(F->typeInfer(K));
                   return F;
                 });
}
}
