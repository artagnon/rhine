#include "rhine/IR/Type.hpp"
#include "rhine/Diagnostic/Diagnostic.hpp"
#include "rhine/IR/Constant.hpp"
#include "rhine/IR/Context.hpp"
#include "rhine/IR/Function.hpp"
#include "rhine/IR/GlobalValue.hpp"
#include "rhine/IR/Instruction.hpp"
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
    return GlobalString::get(std::to_string(I->val()), K);
  if (!isa<IntegerType>(V->getRTy()))
    return nullptr;
  auto ToStringTy =
      FunctionType::get(StringType::get(K), {IntegerType::get(32, K)}, false);
  auto ToStringF = K->Map.get(Prototype::get("toString", ToStringTy));
  auto Op = CallInst::get(ToStringF, {V});
  Op->setType(PointerType::get(ToStringTy));
  return Op;
}

Value *TypeCoercion::convertValue(Value *V, BoolType *) { return V; }

Value *TypeCoercion::convertValue(Value *V, Type *Ty) {
  if (V->getRTy() == Ty)
    return V;
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
      continue;
    }
    std::ostringstream ErrMsg;
    ErrMsg << "Unable to coerce argument from " << *V->getRTy() << " to "
           << *DestTy;
    DiagnosticPrinter(V->getSourceLocation()) << ErrMsg.str();
    exit(1);
  }
}

void TypeCoercion::transformInstruction(Instruction *I) {
  switch (I->op()) {
  case RT_CallInst:
    convertOperands(cast<User>(I), cast<CallInst>(I)->getATys());
    return;
  case RT_IfInst: {
    auto Cond = cast<IfInst>(I)->getConditional();
    Use *CondUse = *Cond;
    if (auto ConvertedConditional = convertValue(Cond, BoolType::get(K))) {
      CondUse->set(ConvertedConditional);
      return;
    }
    std::ostringstream ErrMsg;
    ErrMsg << "Unable to coerce conditional from type " << *Cond->getType()
           << " to Bool";
    DiagnosticPrinter(Cond->getSourceLocation()) << ErrMsg.str();
    exit(1);
  }
  case RT_ReturnInst:
    convertOperands(cast<User>(I), {I->getRTy()});
  default:
    return;
  }
}

void TypeCoercion::runOnFunction(Function *F) {
  K = F->getContext();
  for (auto &BB : *F)
    for (auto &I : *BB)
      transformInstruction(I);
}
}
