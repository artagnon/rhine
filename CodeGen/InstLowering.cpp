#include "llvm/ADT/None.h"

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

llvm::Value *getCalleeFunction(std::string Name, location SourceLoc,
                               llvm::Module *M, Context *K) {
  if (auto Result = K->getMappingVal(Name)) {
    if (isa<llvm::Function>(Result) || isPointerToFunction(Result))
      return Result;
    else {
      auto Sym = LoadInst::get(Name, K->getMappingTy(Name), K)->toLL(M, K);
      if (isPointerToFunction(Sym))
        return Sym;
      K->DiagPrinter->errorReport(
          SourceLoc, Name + " was not declared as a function");
      exit(1);
    }
  } else if (auto FCandidate = Externals::get(K)->getMappingVal(Name, M)) {
    if (auto CalleeCandidate = dyn_cast<llvm::Function>(FCandidate))
      return CalleeCandidate;
    else {
      K->DiagPrinter->errorReport(
          SourceLoc, Name + " was declared with different signature earlier");
      exit(1);
    }
  }
  K->DiagPrinter->errorReport(
      SourceLoc, "unable to look up function " + Name);
  exit(1);
}

llvm::Value *CallInst::toLL(llvm::Module *M, Context *K) {
  auto CalleeFn = getCalleeFunction(Callee, SourceLoc, M, K);
  auto RTy = cast<FunctionType>(cast<PointerType>(VTy)->getCTy())->getRTy();

  if (!getOperands().size()) {
    if (isa<VoidType>(RTy)) {
      K->Builder->CreateCall(CalleeFn);
      return nullptr;
    }
    return K->Builder->CreateCall(CalleeFn, llvm::None, Name);
  }

  // Prepare arguments to call
  std::vector<llvm::Value *> LLOps;
  for (auto Op: getOperands()) {
    LLOps.push_back(Op->toLL(M, K));
  }
  if (isa<VoidType>(RTy)) {
    K->Builder->CreateCall(CalleeFn, LLOps);
    return nullptr;
  }
  return K->Builder->CreateCall(CalleeFn, LLOps, Name);
}

llvm::Value *AddInst::toLL(llvm::Module *M, Context *K) {
  auto Op0 = getOperand(0)->toLL(M, K);
  auto Op1 = getOperand(1)->toLL(M, K);
  return K->Builder->CreateAdd(Op0, Op1);
}

llvm::Value *MallocInst::toLL(llvm::Module *M, Context *K) {
  auto V = getVal()->toLL(M, K);
  auto Ty = getVal()->getType()->toLL(M, K);
  auto Name = getName();
  auto Sz = Ty->getPrimitiveSizeInBits() / 8;
  if (!Sz) Sz = 1;
  auto ITy = IntegerType::get(64, K)->toLL(M, K);
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

llvm::Value *LoadInst::toLL(llvm::Module *M, Context *K) {
  auto Name = getName();
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

llvm::Value *IfInst::toLL(llvm::Module *M, Context *K) {
  auto TrueBB = llvm::BasicBlock::Create(
      K->LLContext, "true", K->CurrentFunction);
  auto FalseBB = llvm::BasicBlock::Create(
      K->LLContext, "false", K->CurrentFunction);
  auto MergeBB = llvm::BasicBlock::Create(
      K->LLContext, "merge", K->CurrentFunction);
  auto Conditional = getConditional()->toLL(M, K);
  K->Builder->CreateCondBr(Conditional, TrueBB, FalseBB);

  K->Builder->SetInsertPoint(TrueBB);
  auto TrueV = getTrueBB()->toLL(M, K);
  K->Builder->CreateBr(MergeBB);
  TrueBB = K->Builder->GetInsertBlock();

  K->Builder->SetInsertPoint(FalseBB);
  auto FalseV = getFalseBB()->toLL(M, K);
  K->Builder->CreateBr(MergeBB);
  FalseBB = K->Builder->GetInsertBlock();

  K->Builder->SetInsertPoint(MergeBB);
  if (!isa<VoidType>(VTy)) {
    auto PN = K->Builder->CreatePHI(VTy->toLL(M, K), 2, "iftmp");
    PN->addIncoming(TrueV, TrueBB);
    PN->addIncoming(FalseV, FalseBB);
    return PN;
  }
  return nullptr;
}
}
