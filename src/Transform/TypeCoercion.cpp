#include "rhine/IR/GlobalValue.hpp"
#include "rhine/IR/Instruction.hpp"
#include "rhine/IR/Constant.hpp"
#include "rhine/IR/Function.hpp"
#include "rhine/IR/Context.hpp"
#include "rhine/IR/Type.hpp"
#include "rhine/Diagnostic/Diagnostic.hpp"
#include "rhine/Transform/TypeCoercion.hpp"

namespace rhine {
TypeCoercion::TypeCoercion() : K(nullptr) {}

Value *TypeCoercion::convertValue(ConstantInt *I, IntegerType *DestTy) {
  auto SourceTy = cast<IntegerType>(I->getRTy());
  auto SourceBitwidth = SourceTy->getBitwidth();
  auto TargetBitwidth = DestTy->getBitwidth();
  if (SourceBitwidth != TargetBitwidth)
    I->setType(IntegerType::get(TargetBitwidth, K));
  return I;
}

Value *TypeCoercion::convertValue(Value *V, StringType *) {
  if (auto I = dyn_cast<ConstantInt>(V))
    return GlobalString::get(std::to_string(I->getVal()), K);
  if (!isa<IntegerType>(V->getRTy()))
    return nullptr;
  auto ToStringTy =
    FunctionType::get(StringType::get(K), {IntegerType::get(32, K)}, false);
  auto ToStringF = K->Map.get(Prototype::get("toString", ToStringTy));
  auto Op = CallInst::get(ToStringF, {V});
  Op->setType(PointerType::get(ToStringTy));
  return Op;
}

Value *TypeCoercion::convertValue(Value *V, BoolType *) {
  return V;
}

Value *TypeCoercion::convertValue(Value *V, Type *Ty) {
  if (V->getRTy() == Ty) return V;
  if (auto STy = dyn_cast<StringType>(Ty))
    return convertValue(V, STy);
  if (auto I = dyn_cast<ConstantInt>(V)) {
    if (auto ITy = dyn_cast<IntegerType>(Ty))
      return convertValue(I, ITy);
  }
  return nullptr;
}

void TypeCoercion::convertOperands(User *U, std::vector<Type *> Tys) {
  auto TyIt = Tys.begin();
  for (Use &ThisUse : U->operands()) {
    Value *V = ThisUse;
    auto DestTy = *TyIt++;
    if (auto ConvertedOp = convertValue(V, DestTy)) {
      ThisUse.set(ConvertedOp);
    } else {
      std::ostringstream MismatchedType;
      MismatchedType << "Unable to coerce argument from "
                     << *V->getRTy() << " to " << *DestTy;
      K->DiagPrinter->errorReport(V->getSourceLocation(), MismatchedType.str());
      exit(1);
    }
  }
}

void TypeCoercion::transformInstruction(Instruction *I) {
  if (auto Inst = dyn_cast<CallInst>(I)) {
    convertOperands(cast<User>(Inst), Inst->getATys());
  }
  else if (auto Inst = dyn_cast<IfInst>(I)) {
    auto Cond = Inst->getConditional();
    Use *CondUse = *Cond;
    if (auto ConvertedConditional = convertValue(Cond, BoolType::get(K))) {
      CondUse->set(ConvertedConditional);
      return;
    }
    std::ostringstream MismatchedType;
    MismatchedType << "Unable to coerce conditional from type "
                   << *Cond->getType() << " to Bool";
    K->DiagPrinter->errorReport(Cond->getSourceLocation(), MismatchedType.str());
    exit(1);
  }
  else if (auto Inst = dyn_cast<ReturnInst>(I)) {
    convertOperands(cast<User>(Inst), { I->getRTy() });
  }
}

void TypeCoercion::runOnFunction(Function *F) {
  K = F->getContext();
  for (auto &BB : *F)
    for (auto &I : *BB)
      transformInstruction(I);
}
}
