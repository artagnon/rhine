#include "rhine/IR/Context.hpp"
#include "rhine/Externals.hpp"
#include "rhine/IR/Tensor.hpp"

#include <iostream>

namespace rhine {
llvm::Value *Tensor::toLL(llvm::Module *M) {
  auto K = context();
  auto ElTy = type()->containedType()->toLL(M);
  auto PElTy = llvm::PointerType::get(ElTy, 0);
  auto Dims = type()->getDims();
  auto NElements = 1;
  for (auto Dim : Dims)
    NElements *= Dim;
  auto ConstElts = ConstantInt::get(NElements, 64, K)->toLL(M);
  auto ConstPtrSize = ConstantInt::get(8, 64, K)->toLL(M);
  auto TensorLen = K->Builder->CreateMul(ConstElts, ConstPtrSize);
  auto MallocF = Externals::get(K)->mappingVal("malloc", M);
  auto Slot = K->Builder->CreateCall(MallocF, {TensorLen}, "TensorAlloc");
  auto CastSlot = K->Builder->CreateBitCast(Slot, PElTy);
  auto PtrN = [K, M, CastSlot](size_t N) {
    auto Idx = ConstantInt::get(N, 32, K)->toLL(M);
    return K->Builder->CreateInBoundsGEP(CastSlot, Idx);
  };
  for (auto Idx = 0; Idx < NElements; Idx++) {
    auto ElToStore = getElts()[Idx]->toLL(M);
    auto SlotToStoreIn = PtrN(Idx);
    K->Builder->CreateStore(ElToStore, SlotToStoreIn);
  }
  return PtrN(0);
}
}
