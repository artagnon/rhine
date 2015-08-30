#include "rhine/IR/Constant.h"
#include "rhine/IR/BasicBlock.h"
#include "rhine/IR/Instruction.h"
#include "rhine/IR/GlobalValue.h"
#include "rhine/Transform/TypeInfer.h"

namespace rhine {
TypeInfer::TypeInfer() : K(nullptr) {}

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
    return VoidType::get(K);
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

Type *TypeInfer::visit(Prototype *V) {
  return V->getType();
}

Type *TypeInfer::visit(Function *V) {
  auto FTy = cast<FunctionType>(V->getType());
  if (isa<UnType>(FTy->getRTy())) {
    Type *LastTy = nullptr;
    for (auto BB : *V)
      LastTy = typeInferValueList(BB->ValueList);
    assert(LastTy && "Function has null body");
    FTy = FunctionType::get(LastTy, FTy->getATys(), false, K);
    V->setType(FTy);
  }
  K->Map.add(V);
  return FTy;
}

Type *TypeInfer::visit(Pointer *V) {
  auto Ty = PointerType::get(visit(V->getVal()), K);
  V->setType(Ty);
  return Ty;
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
  auto TrueBlock = V->getTrueBB();
  auto FalseBlock = V->getFalseBB();
  auto TrueTy = typeInferBB(TrueBlock);
  auto FalseTy = typeInferBB(FalseBlock);
  if (TrueTy != FalseTy) {
    K->DiagPrinter->errorReport(
        V->getSourceLocation(), "mismatched true/false block types");
    exit(1);
  }
  V->setType(TrueTy);
  return TrueTy;
}

Type *TypeInfer::visit(LoadInst *V) {
  if (auto W = K->Map.get(V)) {
    auto Ty = W->getType();
    V->setType(Ty);
    K->Map.add(V);
    return Ty;
  }
  auto Name = V->getVal()->getName();
  K->DiagPrinter->errorReport(V->getSourceLocation(),
                              "untyped symbol " + Name);
  exit(1);
}

Type *TypeInfer::visit(Argument *V) {
  if (auto W = K->Map.get(V)) {
    auto Ty = W->getType();
    V->setType(Ty);
    return Ty;
  }
  K->DiagPrinter->errorReport(V->getSourceLocation(),
                              "untyped argument " + V->getName());
  exit(1);
}

Type *TypeInfer::visit(CallInst *V) {
  typeInferValueList(V->getOperands());
  auto Callee = V->getCallee();
  auto CalleeTy = visit(Callee);
  assert(!isa<UnresolvedValue>(Callee));
  if (auto P = dyn_cast<Pointer>(Callee))
    Callee = P->getVal();
  else if (auto PTy = dyn_cast<PointerType>(CalleeTy))
    Callee->setType(PTy->getCTy());
  CalleeTy = Callee->getType();
  if (auto Ty = dyn_cast<FunctionType>(CalleeTy)) {
    V->setType(PointerType::get(Ty, K));
    return Ty->getRTy();
  }
  auto NotTypedAsFunction =
    Callee->getName() + " was not typed as a function";
  K->DiagPrinter->errorReport(V->getSourceLocation(), NotTypedAsFunction);
  exit(1);
}

Type *TypeInfer::visit(ReturnInst *V) {
  if (!V->getVal())
    return VoidType::get(K);
  auto Ty = visit(V->getVal());
  V->setType(Ty);
  return Ty;
}

Type *TypeInfer::visit(MallocInst *V) {
  V->setType(visit(V->getVal()));
  assert (!V->isUnTyped() && "Unable to type infer MallocInst");
  K->Map.add(V);
  return VoidType::get(K);
}

void TypeInfer::runOnFunction(Function *F) {
  K = F->getContext();
  visit(F);
}
}
