#include "rhine/IR.h"
#include "rhine/Transform/TypeCoercion.h"

namespace rhine {
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
    auto FTy = cast<FunctionType>(C->getType());
    if (dyn_cast<StringType>(FTy->getRTy()))
      return V;
  }
  assert(0 && "Unable to construct GlobalString");
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

void TypeCoercion::runOnFunction(Function *F) {
  std::transform(
      F->begin(), F->end(), F->begin(),
      [this](Value *V) -> Value * {
        if (auto C = dyn_cast<CallInst>(V)) {
          auto FTy = cast<FunctionType>(C->getType());
          auto OpSize = C->getOperands().size();
          auto ASize = FTy->getATys().size();
          auto SourceLoc = C->getSourceLocation();
          if (OpSize != ASize) {
            K->DiagPrinter->errorReport(
                SourceLoc, "CallInst arguments size mismatch: " +
                std::to_string(OpSize) + " versus " + std::to_string(ASize));
            exit(1);
          }
          std::vector<Value *> TransformedOperands;
          for (unsigned long It = 0; It < ASize; It++)
            TransformedOperands.push_back(
                convertValue(C->getOperand(It), FTy->getATy(It)));
          C->setOperands(TransformedOperands);
          return C;
        }
        return V;
      });
}

void TypeCoercion::runOnModule(Module *M) {
  for (auto F: *M)
    runOnFunction(F);
}
}
