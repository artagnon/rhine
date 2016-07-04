#include "llvm/IR/DataLayout.h"

#include "rhine/Diagnostic/Diagnostic.hpp"
#include "rhine/Externals.hpp"
#include "rhine/IR/Context.hpp"
#include "rhine/IR/Instruction.hpp"

#include <iostream>

namespace rhine {
llvm::Value *CallInst::toLL(llvm::Module *M) {
  CHECK_LoweredValue;
  auto K = context();
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
  returni(K->Builder->CreateCall(CalleeFn, LLOps, Name));
}

llvm::Value *BinaryArithInst::toLL(llvm::Module *M) {
  CHECK_LoweredValue;
  auto K = context();
  auto Op0 = getOperand(0)->toLL(M);
  auto Op1 = getOperand(1)->toLL(M);
  switch (op()) {
  case RT_AddInst:
    returni(K->Builder->CreateAdd(Op0, Op1));
  case RT_SubInst:
    returni(K->Builder->CreateSub(Op0, Op1));
  case RT_MulInst:
    returni(K->Builder->CreateMul(Op0, Op1));
  case RT_DivInst:
    returni(K->Builder->CreateSDiv(Op0, Op1));
  default:
    assert(0 && "Malformed BinaryArithInst; cannot lower");
  }
  return nullptr;
}

llvm::Value *BindInst::toLL(llvm::Module *M) {
  CHECK_LoweredValue;
  returni(getOperand(0)->toLL(M));
}

llvm::Value *MallocInst::toLL(llvm::Module *M) {
  CHECK_LoweredValue;
  auto K = context();
  auto V = val()->toLL(M);
  auto RhTy = val()->type();
  auto Ty = RhTy->toLL(M);
  auto DL = DataLayout(M);
  auto Sz = DL.getTypeSizeInBits(Ty) / 8;
  if (!Sz)
    Sz = 1;
  auto ITy = IntegerType::get(64, K)->toLL(M);
  auto CallArg = llvm::ConstantInt::get(ITy, Sz);
  auto MallocF = Externals::get(K)->mappingVal("malloc", M);
  auto Slot = K->Builder->CreateCall(MallocF, {CallArg}, "Alloc");
  auto CastSlot =
      K->Builder->CreateBitCast(Slot, llvm::PointerType::get(Ty, 0));
  K->Builder->CreateStore(V, CastSlot);
  returni(CastSlot);
}

llvm::Value *StoreInst::toLL(llvm::Module *M) {
  CHECK_LoweredValue;
  auto K = context();
  auto Op0 = getMallocedValue();
  if (auto MValue = Op0->getLoweredValue()) {
    auto NewValue = getNewValue()->toLL(M);
    returni(K->Builder->CreateStore(NewValue, MValue));
  }
  DiagnosticPrinter(sourceLocation())
      << "unable to find symbol " + Op0->getName() + " to store into";
  exit(1);
}

llvm::Value *LoadInst::toLL(llvm::Module *M) {
  CHECK_LoweredValue;
  auto K = context();
  if (auto Result = val()->getLoweredValue()) {
    returni(K->Builder->CreateLoad(Result, Name + "Load"));
  } else if (auto Result = Externals::get(K)->mappingVal(Name, M))
    return Result;
  DiagnosticPrinter(SourceLoc) << "unable to find " + Name + " to load";
  exit(1);
}

llvm::Value *ReturnInst::toLL(llvm::Module *M) {
  CHECK_LoweredValue;
  auto K = context();
  if (auto ReturnVal = val())
    return K->Builder->CreateRet(ReturnVal->toLL(M));
  returni(K->Builder->CreateRet(nullptr));
}

llvm::Value *TerminatorInst::toLL(llvm::Module *M) {
  CHECK_LoweredValue;
  returni(val()->toLL(M));
}

llvm::Value *IfInst::toLL(llvm::Module *M) {
  CHECK_LoweredValue;
  returni(getParent()->getPhiValueFromBranchBlock(M));
}

static size_t multiplyDown(std::vector<size_t> &Dims, size_t Top) {
  size_t Mul = 1;
  for (size_t i = Top; i < Dims.size(); ++i) {
    Mul *= Dims[i];
  }
  return Mul;
}

llvm::Value *IndexingInst::toLL(llvm::Module *M) {
  CHECK_LoweredValue;
  auto K = context();
  auto BoundValue = cast<BindInst>(val());
  auto Op0 = BoundValue->val();
  auto Indices = getIndices();
  if (auto IndexingInto = BoundValue->getLoweredValue()) {
    llvm::Value *SumIdx = ConstantInt::get(0, 32, K)->toLL(M);
    auto Dims = cast<TensorType>(Op0->type())->getDims();
    Dims.erase(Dims.begin());
    Dims.push_back(1);
    for (size_t i = 0; i < Dims.size(); i++) {
      auto ToMul = multiplyDown(Dims, i);
      auto Muller = ConstantInt::get(ToMul, 32, K)->toLL(M);
      auto Adder = K->Builder->CreateMul(Indices[i]->toLL(M), Muller);
      SumIdx = K->Builder->CreateAdd(SumIdx, Adder);
    }
    auto ToLoad = K->Builder->CreateInBoundsGEP(IndexingInto, SumIdx);
    returni(K->Builder->CreateLoad(ToLoad, BoundValue->getName() + "Load"));
  }
  DiagnosticPrinter(BoundValue->sourceLocation())
      << "unable to find symbol " + BoundValue->getName() + " to index into";
  exit(1);
}
}
