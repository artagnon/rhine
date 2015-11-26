#include "llvm/IR/DataLayout.h"

#include "rhine/IR/Context.h"
#include "rhine/IR/Instruction.h"
#include "rhine/Externals.h"
#include "rhine/Diagnostic/Diagnostic.h"

namespace rhine {
llvm::Value *CallInst::toLL(llvm::Module *M) {
  auto K = getContext();
  auto RTy = cast<FunctionType>(cast<PointerType>(VTy)->getCTy())->getRTy();
  auto CalleeFn = getCallee()->toLL(M);

  // Prepare arguments to call
  std::vector<llvm::Value *> LLOps;
  for (auto Op : getOperands()) {
    LLOps.push_back(Op->toLL(M));
  }
  if (isa<VoidType>(RTy)) {
    K->Builder->CreateCall(CalleeFn, LLOps);
    return nullptr;
  }
  return K->Builder->CreateCall(CalleeFn, LLOps, Name);
}

llvm::Value *AddInst::toLL(llvm::Module *M) {
  auto K = getContext();
  auto Op0 = getOperand(0)->toLL(M);
  auto Op1 = getOperand(1)->toLL(M);
  return K->Builder->CreateAdd(Op0, Op1);
}

llvm::Value *SubInst::toLL(llvm::Module *M) {
  auto K = getContext();
  auto Op0 = getOperand(0)->toLL(M);
  auto Op1 = getOperand(1)->toLL(M);
  return K->Builder->CreateSub(Op0, Op1);
}

llvm::Value *MallocInst::toLL(llvm::Module *M) {
  auto K = getContext();
  auto V = getVal()->toLL(M);
  auto Ty = getVal()->getType()->toLL(M);
  auto DL = DataLayout(M);
  auto Sz = DL.getTypeSizeInBits(Ty) / 8;
  if (!Sz)
    Sz = 1;
  auto ITy = IntegerType::get(64, K)->toLL(M);
  std::vector<llvm::Value *> LLOps;
  LLOps.push_back(llvm::ConstantInt::get(ITy, Sz));
  auto MallocF = Externals::get(K)->getMappingVal("malloc", M);
  auto Slot = K->Builder->CreateCall(MallocF, LLOps, "Alloc");
  auto CastSlot =
      K->Builder->CreateBitCast(Slot, llvm::PointerType::get(Ty, 0));
  K->Builder->CreateStore(V, CastSlot);
  assert(K->Map.add(this, CastSlot) &&
         (Name + " lowered to a different value earlier").c_str());
  return nullptr;
}

llvm::Value *StoreInst::toLL(llvm::Module *M) {
  auto K = getContext();
  auto Op0 = getMallocedValue();
  if (auto MValue = K->Map.getl(Op0)) {
    auto NewValue = getNewValue()->toLL(M);
    return K->Builder->CreateStore(NewValue, MValue);
  }
  auto Error = "unable to find symbol " + Op0->getName() + " to store into";
  K->DiagPrinter->errorReport(SourceLoc, Error);
  exit(1);
}

llvm::Value *LoadInst::toLL(llvm::Module *M) {
  auto K = getContext();
  if (auto Result = K->Map.getl(getVal())) {
    return K->Builder->CreateLoad(Result, Name + "Load");
  } else if (auto Result = Externals::get(K)->getMappingVal(Name, M))
    return Result;
  auto Error = "unable to find " + Name + " to load";
  K->DiagPrinter->errorReport(SourceLoc, Error);
  exit(1);
}

llvm::Value *ReturnInst::toLL(llvm::Module *M) {
  auto K = getContext();
  if (auto ReturnVal = getVal())
    return K->Builder->CreateRet(ReturnVal->toLL(M));
  return K->Builder->CreateRet(nullptr);
}

llvm::Value *IfInst::toLL(llvm::Module *M) {
  return getParent()->getPhiValueFromBranchBlock(M);
}
}
