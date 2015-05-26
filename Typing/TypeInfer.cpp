#include "rhine/IR.h"
#include "rhine/Resolve.h"

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

void typeInferSymbolList(std::vector<Symbol *> V, Context *K) {
  std::transform(V.begin(), V.end(), V.begin(),
                 [K](Symbol *S) -> Symbol * {
                   S->typeInfer(K);
                   return S;
                 });
}

Type *typeInferValueList(std::vector<Value *> V, Context *K) {
  std::transform(V.begin(), V.end(), V.begin(),
                 [K](Value *L) -> Value * {
                   L->typeInfer(K);
                   return L;
                 });
  return V.back()->typeInfer(K);
}

Type *Lambda::typeInfer(Context *K) {
  typeInferSymbolList(getArguments(), K);
  auto LastTy = typeInferValueList(getVal(), K);
  auto FTy = FunctionType::get(
      LastTy, cast<FunctionType>(getType())->getATys(), K);
  setType(FTy);
  return FTy;
}

Type *Function::typeInfer(Context *K) {
  typeInferSymbolList(getArguments(), K);
  auto LastTy = typeInferValueList(getVal(), K);
  auto FTy = FunctionType::get(
      LastTy, cast<FunctionType>(getType())->getATys(), K);
  setType(FTy);
  K->addMapping(getName(), FTy);
  return FTy;
}

Type *AddInst::typeInfer(Context *K) {
  auto Operand0 = getOperand(0);
  auto Operand1 = getOperand(1);
  auto LType = Operand0->typeInfer(K);
  if (Operand1->typeInfer(K) != LType)
    assert(0 && "AddInst with operands of different types");
  Operand0->setType(LType);
  Operand1->setType(LType);
  setType(FunctionType::get(LType, {LType, LType}, K));
  return LType;
}

Type *Symbol::typeInfer(Context *K) {
  if (auto Ty = Resolve::resolveSymbolTy(getName(), getType(), K)) {
    setType(Ty);
    if (isa<FunctionType>(Ty)) {
      auto PTy = PointerType::get(Ty, K);
      K->addMapping(Name, PTy);
      return PTy;
    }
    K->addMapping(Name, Ty);
    return Ty;
  }
  K->DiagPrinter->errorReport(
      SourceLoc, "untyped symbol " + Name);
  exit(1);
}

Type *CallInst::typeInfer(Context *K) {
  if (auto SymTy = Resolve::resolveSymbolTy(getName(), getType(), K)) {
    if (auto Ty = dyn_cast<FunctionType>(SymTy)) {
      setType(Ty);
      return Ty->getRTy();
    }
    K->DiagPrinter->errorReport(
        SourceLoc, Name + " was not typed as a function");
    exit(1);
  }
  K->DiagPrinter->errorReport(
      SourceLoc, "untyped function " + Name);
  exit(1);
}

Type *BindInst::typeInfer(Context *K) {
  Type *Ty = getVal()->typeInfer(K);
  assert (!isa<UnType>(Ty) && "Unable to type infer BindInst");
  K->addMapping(getName(), Ty);
  return getType();
}

void Module::typeInfer(Context *K) {
  auto V = getVal();
  std::transform(V.begin(), V.end(), V.begin(),
                 [K](Function *F) -> Function * {
                   F->typeInfer(K);
                   return F;
                 });
}
}
