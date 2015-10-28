#include "rhine/IR/GlobalValue.h"
#include "rhine/IR/Instruction.h"
#include "rhine/IR/Constant.h"
#include "rhine/IR/Context.h"
#include "rhine/IR/Value.h"
#include "rhine/IR/Type.h"
#include "rhine/Transform/TypeCoercion.h"

namespace rhine {
TypeCoercion::TypeCoercion() : K(nullptr) {}

Value *TypeCoercion::convertValue(ConstantInt *I, IntegerType *DestTy) {
  auto SourceTy = cast<IntegerType>(I->getType());
  auto SourceBitwidth = SourceTy->getBitwidth();
  auto TargetBitwidth = DestTy->getBitwidth();
  if (SourceBitwidth != TargetBitwidth)
    I->setType(IntegerType::get(TargetBitwidth, K));
  return I;
}

Value *TypeCoercion::convertValue(Value *V, StringType *) {
  if (isa<StringType>(V->getType()))
    return V;
  if (auto I = dyn_cast<ConstantInt>(V))
    return GlobalString::get(std::to_string(I->getVal()), K);
  if (auto C = dyn_cast<CallInst>(V)) {
    auto PTy = cast<PointerType>(C->getType());
    auto FTy = cast<FunctionType>(PTy->getCTy());
    if (dyn_cast<StringType>(FTy->getRTy()))
      return V;
  }
  auto ToStringTy =
    FunctionType::get(StringType::get(K), {IntegerType::get(32, K)}, false);
  auto ToStringF = K->Map.get(Prototype::get("toString", ToStringTy));
  auto Op = CallInst::get(ToStringF, {V});
  Op->setType(PointerType::get(ToStringTy));
  return Op;
}

Value *TypeCoercion::convertValue(Value *V, BoolType *) {
  if (isa<BoolType>(V->getType()))
    return V;
  return V;
}

Value *TypeCoercion::convertValue(Value *V, Type *Ty) {
  if (auto STy = dyn_cast<StringType>(Ty))
    return convertValue(V, STy);
  if (auto I = dyn_cast<ConstantInt>(V)) {
    if (auto ITy = dyn_cast<IntegerType>(Ty))
      return convertValue(I, ITy);
  }
  return V;
}

void TypeCoercion::convertOperands(User *U, std::vector<Type *> Tys) {
  int It = 0;
  for (Use &ThisUse : U->operands()) {
    Value *V = ThisUse;
    ThisUse.set(convertValue(V, Tys[It++]));
    if (auto Inst = dyn_cast<Instruction>(V))
      transformInstruction(Inst);
  }
}

void TypeCoercion::assertActualFormalCount(CallInst *Inst, FunctionType *FTy) {
  auto OpSize = Inst->getOperands().size();
  auto ASize = FTy->getATys().size();
  auto SourceLoc = Inst->getSourceLocation();
  if (OpSize != ASize) {
    K->DiagPrinter->errorReport(
        SourceLoc, "CallInst arguments size mismatch: " +
        std::to_string(OpSize) + " versus " + std::to_string(ASize));
    exit(1);
  }
}

FunctionType *computeFunctionType(CallInst *Inst) {
  FunctionType *FTy;
  if (auto BareFTy = dyn_cast<FunctionType>(Inst->getType())) {
    FTy = BareFTy;
  } else {
    auto PTy = cast<PointerType>(Inst->getType());
    FTy = cast<FunctionType>(PTy->getCTy());
  }
  return FTy;
}

void TypeCoercion::transformInstruction(Instruction *I) {
  if (auto Inst = dyn_cast<CallInst>(I)) {
    auto FTy = computeFunctionType(Inst);
    assertActualFormalCount(Inst, FTy);
    convertOperands(cast<User>(Inst), FTy->getATys());
  }
  else if (auto Inst = dyn_cast<IfInst>(I)) {
    auto Cond = Inst->getConditional();
    Use *CondUse = *Cond;
    CondUse->set(convertValue(Cond, BoolType::get(K)));
  }
  else if (auto Inst = dyn_cast<ReturnInst>(I)) {
    convertOperands(cast<User>(Inst), { I->getType() });
  }
}

void TypeCoercion::runOnFunction(Function *F) {
  K = F->getContext();
  for (auto &BB : *F)
    for (auto &V : *BB)
      if (auto I = dyn_cast<Instruction>(V))
        transformInstruction(I);
}
}
