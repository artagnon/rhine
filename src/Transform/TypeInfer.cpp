#include "rhine/Diagnostic/Diagnostic.hpp"
#include "rhine/IR/BasicBlock.hpp"
#include "rhine/IR/Constant.hpp"
#include "rhine/IR/Context.hpp"
#include "rhine/IR/Function.hpp"
#include "rhine/IR/GlobalValue.hpp"
#include "rhine/IR/Instruction.hpp"
#include "rhine/Transform/TypeInfer.hpp"

namespace rhine {
TypeInfer::TypeInfer() : K(nullptr) {}

Type *TypeInfer::visit(ConstantInt *V) { return V->getType(); }

Type *TypeInfer::visit(ConstantBool *V) { return V->getType(); }

Type *TypeInfer::visit(ConstantFloat *V) { return V->getType(); }

Type *TypeInfer::visit(GlobalString *V) { return V->getType(); }

Type *TypeInfer::visitHeaderBlock(BasicBlock *BB) {
  if (BB->begin() == BB->end())
    return VoidType::get(K);
  std::vector<Instruction *>::iterator It;
  for (It = BB->begin(); std::next(It) != BB->end(); ++It)
    visit(*It);
  return visit(*It);
}

Type *TypeInfer::visit(BasicBlock *BB) {
  auto Ret = visitHeaderBlock(BB);
  if (auto MergeBlock = BB->getMergeBlock())
    return visitHeaderBlock(MergeBlock);
  return Ret;
}

Type *TypeInfer::visit(Prototype *V) { return V->getType(); }

Type *TypeInfer::visit(Function *V) {
  auto FTy = cast<FunctionType>(V->getType());
  auto Ty = visit(V->getEntryBlock());
  if (isa<UnType>(FTy->getRTy())) {
    FTy = FunctionType::get(Ty, FTy->getATys(), false);
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

Type *TypeInfer::visit(BinaryArithInst *V) {
  for (auto Op : V->operands())
    visit(Op);
  auto LType = V->getOperand(0)->getType();
  assert(LType == V->getOperand(1)->getType() &&
         "BinaryArithInst with operands of different types");
  V->setType(LType);
  return LType;
}

Type *TypeInfer::visit(IfInst *V) {
  auto TrueTy = visit(V->getTrueBB());
  auto FalseTy = visit(V->getFalseBB());
  if (TrueTy != FalseTy) {
    std::ostringstream Message;
    Message << "mismatched types: true block is inferred to be of type "
            << *TrueTy << " and false block is inferred to be of type "
            << *FalseTy;
    K->DiagPrinter->errorReport(V->getSourceLocation(), Message.str());
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

FunctionType *TypeInfer::followFcnPointer(Type *CalleeTy) {
  if (auto PointerTy = dyn_cast<PointerType>(CalleeTy))
    return dyn_cast<FunctionType>(PointerTy->getCTy());
  return nullptr;
}

FunctionType *TypeInfer::followFcnPointers(Value *Callee, Location Loc) {
  if (auto Fcn = followFcnPointer(Callee->getType())) {
    while (auto DeeperFcn = followFcnPointer(Fcn))
      Fcn = DeeperFcn;
    return Fcn;
  }
  std::ostringstream NotTypedAsFunction;
  NotTypedAsFunction << Callee->getName()
                     << " was expected to be a pointer to a function"
                     << " but was instead found to be of type "
                     << *Callee->getType();
  K->DiagPrinter->errorReport(Loc, NotTypedAsFunction.str());
  exit(1);
}

void TypeInfer::verifyArity(CallInst *V, FunctionType *Ty) {
  std::vector<Type *> OpTys;
  for (auto Op : V->operands())
    OpTys.push_back(Op->getType());
  auto ATys = Ty->getATys();
  unsigned OpSize = OpTys.size();
  unsigned Arity = ATys.size();
  if (OpSize != Arity) {
    std::ostringstream MismatchedArity;
    MismatchedArity << "Call expected " << Arity
                    << " number of arguments, but has been supplied " << OpSize
                    << " arguments";
    K->DiagPrinter->errorReport(V->getSourceLocation(), MismatchedArity.str());
    exit(1);
  }
#if 0
  auto OpIt = V->operands().begin();
  for (auto FormalArg : ATys) {
    auto Op = *OpIt++;
    auto ActualArg = Op->getType();
    if (FormalArg != ActualArg) {
      std::ostringstream MismatchedArgTy;
      MismatchedArgTy << "Call expected argument of type " << *FormalArg
                      << " but has been supplied " << *ActualArg << " type";
      K->DiagPrinter->errorReport(Op->getSourceLocation(),
                                  MismatchedArgTy.str());
      exit(1);
    }
  }
#endif
}

void TypeInfer::visitCalleeAndOperands(CallInst *V) {
  for (auto Op : V->operands())
    visit(Op);
  auto Callee = V->getCallee();
  visit(Callee);
  assert(!isa<UnresolvedValue>(Callee));
}

Type *TypeInfer::visit(CallInst *V) {
  visitCalleeAndOperands(V);
  auto Callee = V->getCallee();
  auto Ty = followFcnPointers(Callee, V->getSourceLocation());
  verifyArity(V, Ty);
  V->setType(PointerType::get(Ty));
  return Ty->getRTy();
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
