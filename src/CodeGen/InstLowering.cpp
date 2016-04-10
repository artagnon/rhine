#include "llvm/IR/DataLayout.h"

#include "rhine/Diagnostic/Diagnostic.hpp"
#include "rhine/Externals.hpp"
#include "rhine/IR/Context.hpp"
#include "rhine/IR/Instruction.hpp"

#include <iostream>

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

llvm::Value *BinaryArithInst::toLL(llvm::Module *M) {
  auto K = getContext();
  auto Op0 = getOperand(0)->toLL(M);
  auto Op1 = getOperand(1)->toLL(M);
  switch (getValID()) {
  case RT_AddInst:
    return K->Builder->CreateAdd(Op0, Op1);
  case RT_SubInst:
    return K->Builder->CreateSub(Op0, Op1);
  case RT_MulInst:
    return K->Builder->CreateMul(Op0, Op1);
  case RT_DivInst:
    return K->Builder->CreateSDiv(Op0, Op1);
  default:
    assert(0 && "Malformed BinaryArithInst; cannot lower");
  }
  return nullptr;
}

llvm::Value *BindInst::toLL(llvm::Module *M) {
  if (LoweredValue) {
    return LoweredValue;
  }
  auto LLOp = getOperand(0)->toLL(M);
  setLoweredValue(LLOp);
  return LLOp;
}

llvm::Value *MallocInst::toLL(llvm::Module *M) {
  if (LoweredValue) {
    return LoweredValue;
  }
  auto K = getContext();
  auto V = getVal()->toLL(M);
  auto RhTy = getVal()->getType();
  auto Ty = RhTy->toLL(M);
  auto DL = DataLayout(M);
  auto Sz = DL.getTypeSizeInBits(Ty) / 8;
  if (!Sz)
    Sz = 1;
  auto ITy = IntegerType::get(64, K)->toLL(M);
  auto CallArg = llvm::ConstantInt::get(ITy, Sz);
  auto MallocF = Externals::get(K)->getMappingVal("malloc", M);
  auto Slot = K->Builder->CreateCall(MallocF, {CallArg}, "Alloc");
  auto CastSlot =
      K->Builder->CreateBitCast(Slot, llvm::PointerType::get(Ty, 0));
  K->Builder->CreateStore(V, CastSlot);
  setLoweredValue(CastSlot);
  return nullptr;
}

llvm::Value *StoreInst::toLL(llvm::Module *M) {
  auto K = getContext();
  auto Op0 = getMallocedValue();
  if (auto MValue = Op0->getLoweredValue()) {
    auto NewValue = getNewValue()->toLL(M);
    return K->Builder->CreateStore(NewValue, MValue);
  }
  DiagnosticPrinter(getSourceLocation())
      << "unable to find symbol " + Op0->getName() + " to store into";
  exit(1);
}

llvm::Value *LoadInst::toLL(llvm::Module *M) {
  auto K = getContext();
  if (auto Result = getVal()->getLoweredValue()) {
    return K->Builder->CreateLoad(Result, Name + "Load");
  } else if (auto Result = Externals::get(K)->getMappingVal(Name, M))
    return Result;
  DiagnosticPrinter(SourceLoc) << "unable to find " + Name + " to load";
  exit(1);
}

llvm::Value *ReturnInst::toLL(llvm::Module *M) {
  auto K = getContext();
  if (auto ReturnVal = getVal())
    return K->Builder->CreateRet(ReturnVal->toLL(M));
  return K->Builder->CreateRet(nullptr);
}

llvm::Value *TerminatorInst::toLL(llvm::Module *M) { return getVal()->toLL(M); }

llvm::Value *IfInst::toLL(llvm::Module *M) {
  return getParent()->getPhiValueFromBranchBlock(M);
}

static size_t multiplyDown(std::vector<size_t> &Dims, size_t Top) {
  size_t Mul = 1;
  for (size_t i = Top; i < Dims.size(); ++i) {
    Mul *= Dims[i];
  }
  return Mul;
}

llvm::Value *IndexingInst::toLL(llvm::Module *M) {
  auto K = getContext();
  auto BoundValue = cast<BindInst>(getVal());
  auto Op0 = BoundValue->getVal();
  auto Indices = getIndices();
  if (auto IndexingInto = BoundValue->getLoweredValue()) {
    llvm::Value *SumIdx = ConstantInt::get(0, 32, K)->toLL(M);
    auto Dims = cast<TensorType>(Op0->getType())->getDims();
    Dims.erase(Dims.begin());
    Dims.push_back(1);
    for (size_t i = 0; i < Dims.size(); i++) {
      auto ToMul = multiplyDown(Dims, i);
      auto Muller = ConstantInt::get(ToMul, 32, K)->toLL(M);
      auto Adder = K->Builder->CreateMul(Indices[i]->toLL(M), Muller);
      SumIdx = K->Builder->CreateAdd(SumIdx, Adder);
    }
    auto ToLoad = K->Builder->CreateInBoundsGEP(IndexingInto, SumIdx);
    return K->Builder->CreateLoad(ToLoad, BoundValue->getName() + "Load");
  }
  DiagnosticPrinter(BoundValue->getSourceLocation())
      << "unable to find symbol " + BoundValue->getName() + " to index into";
  exit(1);
}
}
