#include "rhine/IR.h"
#include "rhine/Transform/Resolve.h"

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

template <typename T>
Type *typeInferValueList(std::vector<T> V, Context *K) {
  std::transform(V.begin(), V.end(), V.begin(),
                 [K](T L) -> T {
                   L->typeInfer(K);
                   return L;
                 });
  if (!V.size())
    return nullptr;
  return V.back()->typeInfer(K);
}

Type *Function::typeInfer(Context *K) {
  typeInferValueList(getArguments(), K);
  auto LastTy = typeInferValueList(getVal(), K);
  assert(LastTy && "Function has null body");
  auto FTy = FunctionType::get(
      LastTy, cast<FunctionType>(getType())->getATys(), false, K);
  auto PTy = PointerType::get(FTy, K);
  setType(FTy);
  K->addMapping(Name, PTy);
  return PTy;
}

Type *AddInst::typeInfer(Context *K) {
  typeInferValueList(getOperands(), K);
  auto LType = getOperand(0)->getType();
  assert(LType == getOperand(1)->getType() &&
         "AddInst with operands of different types");
  setType(FunctionType::get(LType, {LType, LType}, false, K));
  return LType;
}

Type *Symbol::typeInfer(Context *K) {
  if (auto Ty = Resolve::resolveSymbolTy(Name, VTy, K)) {
    setType(Ty);
    K->addMapping(Name, Ty);
    return Ty;
  }
  K->DiagPrinter->errorReport(
      SourceLoc, "untyped symbol " + Name);
  exit(1);
}

Type *CallInst::typeInfer(Context *K) {
  typeInferValueList(getOperands(), K);
  if (auto SymTy = Resolve::resolveSymbolTy(Name, VTy, K)) {
    if (auto PTy = dyn_cast<PointerType>(SymTy)) {
      if (auto Ty = dyn_cast<FunctionType>(PTy->getCTy())) {
        setType(PTy);
        return Ty->getRTy();
      }
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
