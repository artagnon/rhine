#include "rhine/Diagnostic/Diagnostic.h"
#include "rhine/IR/BasicBlock.h"
#include "rhine/IR/Constant.h"
#include "rhine/IR/Context.h"
#include "rhine/IR/GlobalValue.h"
#include "rhine/IR/Instruction.h"
#include "rhine/Transform/TypeInfer.h"

namespace rhine {
TypeInfer::TypeInfer() : K(nullptr) {}

Type *TypeInfer::visit(ConstantInt *V) { return V->getType(); }

Type *TypeInfer::visit(ConstantBool *V) { return V->getType(); }

Type *TypeInfer::visit(ConstantFloat *V) { return V->getType(); }

Type *TypeInfer::visit(GlobalString *V) { return V->getType(); }

Type *TypeInfer::visit(BasicBlock *BB) {
  if (BB->begin() == BB->end())
    return VoidType::get(K);
  std::vector<Instruction *>::iterator It;
  for (It = BB->begin(); std::next(It) != BB->end(); ++It)
    visit(*It);
  return visit(*It);
}

Type *TypeInfer::visit(Prototype *V) { return V->getType(); }

Type *TypeInfer::visit(Function *V) {
  auto FTy = cast<FunctionType>(V->getType());
  Type *LastTy = VoidType::get(K);
  for (auto BB : *V)
    LastTy = visit(BB);
  if (isa<UnType>(FTy->getRTy())) {
    FTy = FunctionType::get(LastTy, FTy->getATys(), false);
    V->setType(FTy);
  }
  assert(K->Map.add(V) &&
         ("Function with name " + V->getName() + " already declared").c_str());
  return FTy;
}

Type *TypeInfer::visit(Pointer *V) {
  auto Ty = PointerType::get(visit(V->getVal()));
  V->setType(Ty);
  return Ty;
}

Type *TypeInfer::visit(AddInst *V) {
  for (auto Op : V->operands())
    visit(Op);
  auto LType = V->getOperand(0)->getType();
  assert(LType == V->getOperand(1)->getType() &&
         "AddInst with operands of different types");
  V->setType(LType);
  return LType;
}

Type *TypeInfer::visit(SubInst *V) {
  for (auto Op : V->operands())
    visit(Op);
  auto LType = V->getOperand(0)->getType();
  assert(LType == V->getOperand(1)->getType() &&
         "SubInst with operands of different types");
  V->setType(LType);
  return LType;
}

Type *TypeInfer::visit(IfInst *V) {
  auto TrueBlock = V->getTrueBB();
  auto FalseBlock = V->getFalseBB();
  auto TrueTy = visit(TrueBlock);
  auto FalseTy = visit(FalseBlock);
  if (TrueTy != FalseTy) {
    K->DiagPrinter->errorReport(V->getSourceLocation(),
                                "mismatched true/false block types");
    exit(1);
  }
  V->setType(TrueTy);
  return TrueTy;
}

Type *TypeInfer::visit(LoadInst *V) {
  V->setType(V->getVal()->getType());
  if (!V->isUnTyped())
    return V->getType();
  auto Name = V->getVal()->getName();
  K->DiagPrinter->errorReport(V->getSourceLocation(), "untyped symbol " + Name);
  exit(1);
}

Type *TypeInfer::visit(StoreInst *V) {
  auto MallocedV = V->getMallocedValue();
  auto NewTy = visit(V->getNewValue());
  MallocedV->setType(NewTy);
  return VoidType::get(K);
}

Type *TypeInfer::visit(Argument *V) {
  if (!V->isUnTyped())
    return V->getType();
  K->DiagPrinter->errorReport(V->getSourceLocation(),
                              "untyped argument " + V->getName());
  exit(1);
}

Type *TypeInfer::visit(CallInst *V) {
  for (auto Op : V->operands())
    visit(Op);
  auto Callee = V->getCallee();
  auto CalleeTy = visit(Callee);
  assert(!isa<UnresolvedValue>(Callee));
  if (auto P = dyn_cast<Pointer>(Callee))
    Callee = P->getVal();
  else if (auto PTy = dyn_cast<PointerType>(CalleeTy))
    Callee->setType(PTy->getCTy());
  CalleeTy = Callee->getType();
  if (auto Ty = dyn_cast<FunctionType>(CalleeTy)) {
    V->setType(PointerType::get(Ty));
    return Ty->getRTy();
  }
  auto NotTypedAsFunction = Callee->getName() + " was not typed as a function";
  K->DiagPrinter->errorReport(V->getSourceLocation(), NotTypedAsFunction);
  exit(1);
}

Type *TypeInfer::visit(ReturnInst *V) {
  if (!V->getVal())
    return VoidType::get(K);
  auto Val = V->getVal();
  auto Ty = visit(Val);
  if (isa<VoidType>(Ty)) {
    auto CannotReturnVoid = "cannot return expression of Void type";
    K->DiagPrinter->errorReport(Val->getSourceLocation(), CannotReturnVoid);
    exit(1);
  }
  V->setType(Ty);
  return Ty;
}

Type *TypeInfer::visit(TerminatorInst *V) {
  auto Ty = visit(V->getVal());
  V->setType(Ty);
  return Ty;
}

Type *TypeInfer::visit(MallocInst *V) {
  V->setType(visit(V->getVal()));
  assert(!V->isUnTyped() && "unable to type infer MallocInst");
  return VoidType::get(K);
}

void TypeInfer::runOnFunction(Function *F) {
  K = F->getContext();
  visit(F);
}
}
