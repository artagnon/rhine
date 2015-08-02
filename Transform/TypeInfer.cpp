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

Type *TypeInfer::typeInferBB(BasicBlock *BB) {
  std::transform(BB->begin(), BB->end(), BB->begin(),
                 [this](Value *L) -> Value * {
                   visit(L);
                   return L;
                 });
  if (!BB->size())
    return nullptr;
  return visit(BB->back());
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

Type *TypeInfer::visit(IfInst *V) {
   auto TrueTy = typeInferBB(V->getTrueBB());
   auto FalseTy = typeInferBB(V->getFalseBB());
   if (TrueTy != FalseTy) {
     K->DiagPrinter->errorReport(
         V->getSourceLocation(), "mismatched true/false block types");
     exit(1);
   }
   V->setType(TrueTy);
   return TrueTy;
}

Type *TypeInfer::visit(LoadInst *V) {
  auto Name = V->getName();
  auto VTy = V->getType();
  if (auto Ty = Resolve::resolveLoadInstTy(Name, VTy, K)) {
    V->setType(Ty);
    K->addMapping(Name, Ty);
    return Ty;
  }
  K->DiagPrinter->errorReport(
      V->getSourceLocation(), "untyped symbol " + Name);
  exit(1);
}

Type *TypeInfer::visit(Argument *V) {
  auto Name = V->getName();
  auto VTy = V->getType();
  if (auto Ty = Resolve::resolveLoadInstTy(Name, VTy, K)) {
    V->setType(Ty);
    K->addMapping(Name, Ty);
    return Ty;
  }
  K->DiagPrinter->errorReport(
      V->getSourceLocation(), "untyped argument " + Name);
  exit(1);
}

Type *TypeInfer::visit(CallInst *V) {
  typeInferValueList(V->getOperands());
  auto Callee = V->getCallee();
  auto VTy = V->getType();
  if (auto SymTy = Resolve::resolveLoadInstTy(Callee, VTy, K)) {
    if (auto PTy = dyn_cast<PointerType>(SymTy)) {
      if (auto Ty = dyn_cast<FunctionType>(PTy->getCTy())) {
        V->setType(PTy);
        return Ty->getRTy();
      }
    }
    K->DiagPrinter->errorReport(
        V->getSourceLocation(), Callee + " was not typed as a function");
    exit(1);
  }
  K->DiagPrinter->errorReport(
      V->getSourceLocation(), "untyped function " + Callee);
  exit(1);
}

Type *TypeInfer::visit(MallocInst *V) {
  auto Ty = visit(V->getVal());
  assert (!isa<UnType>(Ty) && "Unable to type infer MallocInst");
  K->addMapping(V->getName(), Ty);
  return V->getType();
}

void TypeInfer::runOnFunction(Function *F) {
  visit(F);
}
}
