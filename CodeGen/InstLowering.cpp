#include "llvm/ADT/None.h"
#include "llvm/IR/DataLayout.h"

#include "rhine/Context.h"
#include "rhine/IR/Instruction.h"
#include "rhine/Externals.h"

namespace rhine {
bool isPointerToFunction(llvm::Value *Candidate) {
  if (auto PTy = dyn_cast<llvm::PointerType>(Candidate->getType())) {
    if (isa<llvm::FunctionType>(PTy->getElementType()))
      return true;
  }
  return false;
}

llvm::Value *CallInst::toLL(llvm::Module *M) {
  auto K = getContext();
  auto RTy = cast<FunctionType>(cast<PointerType>(VTy)->getCTy())->getRTy();
  auto Name = getCallee()->getName();
  auto CalleeFn = K->getMappingVal(Name);
  if (!CalleeFn)
    CalleeFn = Externals::get(K)->getMappingVal(Name, M);
  if (!CalleeFn) {
    K->DiagPrinter->errorReport(SourceLoc,
                                "unable to lookup function " + Name);
    exit(1);
  }

  // Prepare arguments to call
  std::vector<llvm::Value *> LLOps;
  for (auto Op: getOperands()) {
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

llvm::Value *MallocInst::toLL(llvm::Module *M) {
  auto K = getContext();
  auto V = getVal()->toLL(M);
  auto Ty = getVal()->getType()->toLL(M);
  auto DL = DataLayout(M);
  auto Sz = DL.getTypeSizeInBits(Ty) / 8;
  if (!Sz) Sz = 1;
  auto ITy = IntegerType::get(64, K)->toLL(M);
  std::vector<llvm::Value *> LLOps;
  LLOps.push_back(llvm::ConstantInt::get(ITy, Sz));
  auto MallocF = Externals::get(K)->getMappingVal("malloc", M);
  auto Slot = K->Builder->CreateCall(MallocF, LLOps, "Alloc");
  auto CastSlot = K->Builder->CreateBitCast(
      Slot, llvm::PointerType::get(Ty, 0));
  K->Builder->CreateStore(V, CastSlot);
  K->addMapping(Name, nullptr, CastSlot);
  return nullptr;
}

llvm::Value *LoadInst::toLL(llvm::Module *M) {
  auto K = getContext();
  if (auto Result = K->getMappingVal(Name)) {
    if (isa<llvm::BitCastInst>(Result))
      return K->Builder->CreateLoad(Result, Name + "Load");
    return Result;
  } else if (auto Result = Externals::get(K)->getMappingVal(Name, M))
    return Result;
  K->DiagPrinter->errorReport(
      SourceLoc, "unbound symbol " + Name);
  exit(1);
}

llvm::Value *ReturnInst::toLL(llvm::Module *M) {
  auto B = getContext()->Builder;
  if (auto ReturnVal = getVal())
    return B->CreateRet(ReturnVal->toLL(M));
  return B->CreateRet(nullptr);
}

llvm::Value *IfInst::toLL(llvm::Module *M) {
  auto K = getContext();
  auto TrueBB = llvm::BasicBlock::Create(
      K->LLContext, "true", K->CurrentFunction);
  auto FalseBB = llvm::BasicBlock::Create(
      K->LLContext, "false", K->CurrentFunction);
  auto MergeBB = llvm::BasicBlock::Create(
      K->LLContext, "merge", K->CurrentFunction);
  auto Conditional = getConditional()->toLL(M);
  K->Builder->CreateCondBr(Conditional, TrueBB, FalseBB);

  K->Builder->SetInsertPoint(TrueBB);
  auto TrueV = getTrueBB()->toLL(M);
  K->Builder->CreateBr(MergeBB);
  TrueBB = K->Builder->GetInsertBlock();

  K->Builder->SetInsertPoint(FalseBB);
  auto FalseV = getFalseBB()->toLL(M);
  K->Builder->CreateBr(MergeBB);
  FalseBB = K->Builder->GetInsertBlock();

  K->Builder->SetInsertPoint(MergeBB);
  if (!isa<VoidType>(VTy)) {
    auto PN = K->Builder->CreatePHI(VTy->toLL(M), 2, "iftmp");
    PN->addIncoming(TrueV, TrueBB);
    PN->addIncoming(FalseV, FalseBB);
    return PN;
  }
  return nullptr;
}
}
