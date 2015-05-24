#include "rhine/IR.h"
#include "rhine/Externals.h"

namespace rhine {
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
  auto ATys = cast<FunctionType>(getType())->getATys();
  Type *LastTy;
  for (auto I: getVal()) {
    LastTy = I->typeInfer(K);
    I->setType(LastTy);
  }
  auto FnTy = FunctionType::get(LastTy, ATys, K);
  setType(FnTy);
  K->addMapping(getName(), FnTy);
  return FnTy;
}

Type *AddInst::typeInfer(Context *K) {
  auto LType = getOperand(0)->typeInfer();
  if (getOperand(1)->typeInfer() != LType)
    assert(0 && "AddInst with operands of different types");
  return getType();
}

Type *generalizedSymbolType(Type *Ty, std::string Name, Context *K) {
  if (Ty != UnType::get(K))
    return Ty;
  if (auto Result = K->getMappingTy(Name)) {
    return Result;
  } else if (auto Result = Externals::get(K)->getMappingTy(Name)) {
    return Result;
  }
  return nullptr;
}

Type *Symbol::typeInfer(Context *K) {
  if (auto Ty = generalizedSymbolType(getType(), getName(), K)) {
    if (FunctionType::classof(Ty)) {
      // First-class function (pointers)
      auto PTy = PointerType::get(Ty, K);
      K->addMapping(Name, PTy);
      return PTy;
    }
    else {
      K->addMapping(Name, Ty);
      return Ty;
    }
  }
  K->DiagPrinter->errorReport(
      SourceLoc, "untyped symbol " + Name);
  exit(1);
}

Type *CallInst::typeInfer(Context *K) {
  if (auto GSymTy = generalizedSymbolType(getType(), getName(), K)) {
    if (auto Ty = dyn_cast<FunctionType>(GSymTy))
      return Ty->getRTy();
    else {
      K->DiagPrinter->errorReport(
          SourceLoc, Name + " was not typed as a function");
      exit(1);
    }
  }
  K->DiagPrinter->errorReport(
      SourceLoc, "untyped function " + Name);
  exit(1);
}

Type *BindInst::typeInfer(Context *K) {
  Type *Ty = getVal()->typeInfer();
  assert (Ty != UnType::get(K) &&
          "Unable to type infer BindInst");
  K->addMapping(getName(), Ty);
  return getType();
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
