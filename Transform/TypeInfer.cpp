#include "rhine/IR.h"
#include "rhine/Transform/TypeInfer.h"
#include "rhine/Transform/Resolve.h"

namespace rhine {
Type *TypeInfer::visit(ConstantInt *V) {
  return V->getType();
}

Type *TypeInfer::visit(ConstantBool *V) {
  return V->getType();
}

Type *TypeInfer::visit(ConstantFloat *V) {
  return V->getType();
}

Type *TypeInfer::visit(GlobalString *V) {
  return V->getType();
}

template <typename T>
Type *TypeInfer::typeInferValueList(std::vector<T> V) {
  std::transform(V.begin(), V.end(), V.begin(),
                 [this](T L) -> T {
                   visit(L);
                   return L;
                 });
  if (!V.size())
    return nullptr;
  return visit(V.back());
}

Type *TypeInfer::visit(Function *V) {
  typeInferValueList(V->getArguments());
  auto LastTy = typeInferValueList(V->getVal()->ValueList);
  assert(LastTy && "Function has null body");
  auto FTy = FunctionType::get(
      LastTy, cast<FunctionType>(V->getType())->getATys(), false, K);
  auto PTy = PointerType::get(FTy, K);
  V->setType(FTy);
  K->addMapping(V->getName(), PTy);
  return PTy;
}

Type *TypeInfer::visit(AddInst *V) {
  typeInferValueList(V->getOperands());
  auto LType = V->getOperand(0)->getType();
  assert(LType == V->getOperand(1)->getType() &&
         "AddInst with operands of different types");
  V->setType(FunctionType::get(LType, {LType, LType}, false, K));
  return LType;
}

Type *TypeInfer::visit(Symbol *V) {
  auto Name = V->getName();
  auto VTy = V->getType();
  if (auto Ty = Resolve::resolveSymbolTy(Name, VTy, K)) {
    V->setType(Ty);
    K->addMapping(Name, Ty);
    return Ty;
  }
  K->DiagPrinter->errorReport(
      V->getSourceLocation(), "untyped symbol " + Name);
  exit(1);
}

Type *TypeInfer::visit(CallInst *V) {
  typeInferValueList(V->getOperands());
  auto Name = V->getName();
  auto VTy = V->getType();
  if (auto SymTy = Resolve::resolveSymbolTy(Name, VTy, K)) {
    if (auto PTy = dyn_cast<PointerType>(SymTy)) {
      if (auto Ty = dyn_cast<FunctionType>(PTy->getCTy())) {
        V->setType(PTy);
        return Ty->getRTy();
      }
    }
    K->DiagPrinter->errorReport(
        V->getSourceLocation(), Name + " was not typed as a function");
    exit(1);
  }
  K->DiagPrinter->errorReport(
      V->getSourceLocation(), "untyped function " + Name);
  exit(1);
}

Type *TypeInfer::visit(BindInst *V) {
  Type *Ty = visit(V->getVal());
  assert (!isa<UnType>(Ty) && "Unable to type infer BindInst");
  K->addMapping(V->getName(), Ty);
  return V->getType();
}

void TypeInfer::runOnModule(Module *M) {
  auto V = M->getVal();
  std::transform(V.begin(), V.end(), V.begin(),
                 [this](Function *F) -> Function * {
                   visit(F);
                   return F;
                 });
}
}
