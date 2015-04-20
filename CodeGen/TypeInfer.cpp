#include "rhine/IR.h"
#include <iostream>

namespace rhine {
//===--------------------------------------------------------------------===//
// Type inference core.
//===--------------------------------------------------------------------===//
Type *Symbol::typeInfer(Context *K) {
  Symbol *V = this;
  if (V->getType() == UnType::get(K)) {
    V = K->getNameMapping(V->getName());
    assert (V->getType() != UnType::get(K) && "Missing seed to infer type");
  }
  K->addNameMapping(V->getName(), V);
  return V->getType();
}

Type *rhine::ConstantInt::typeInfer(Context *K) {
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
  auto V = this->getArgumentList();
  std::transform(V.begin(), V.end(), V.begin(),
                 [K](Symbol *S) -> Symbol * {
                   S->setType(S->typeInfer(K));
                   return S;
                 });
  auto FType = dyn_cast<FunctionType>(this->getType());
  FType->setRTy(this->getVal()->typeInfer(K));
  return FType;
}

Type *AddInst::typeInfer(Context *K) {
  auto LType = this->getOperand(0)->typeInfer();
  if (this->getOperand(1)->typeInfer() != LType)
    assert(0 && "AddInst with operands of different types");
  return this->getType();
}

Type *CallInst::typeInfer(Context *K) {
  return this->getType();
}

void Module::typeInfer(Context *K) {
  auto V = this->getVal();
  std::transform(V.begin(), V.end(), V.begin(),
                 [K](Function *F) -> Function * {
                   F->setType(F->typeInfer(K));
                   return F;
                 });
  this->setVal(V);
}
}
