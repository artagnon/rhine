#include "rhine/IR.h"
#include <iostream>

namespace rhine {
//===--------------------------------------------------------------------===//
// Type inference core.
//===--------------------------------------------------------------------===//
Type *Symbol::typeInfer(Context *K) {
  Type *Ty = getType();
  if (Ty == UnType::get(K))
    Ty = K->getNameTypeMapping(getName(), getSourceLocation());
  K->addNameTypeMapping(getName(), Ty);
  return Ty;
}

Type *ConstantInt::typeInfer(Context *K) {
  return getType();
}

Type *ConstantBool::typeInfer(Context *K) {
  return getType();
}

Type *ConstantFloat::typeInfer(Context *K) {
  return getType();
}

Type *GlobalString::typeInfer(Context *K) {
  return getType();
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
  Type *LastTy;
  for (auto I: getVal()) {
    LastTy = I->typeInfer(K);
    I->setType(LastTy);
  }
  FType->setRTy(LastTy);
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
  Type *Ty = getVal()->typeInfer();
  assert (Ty != UnType::get(K) &&
          "Unable to type infer BindInst");
  K->addNameTypeMapping(getName(), Ty);
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
