#include "rhine/IR/Context.hpp"
#include "rhine/Externals.hpp"
#include "rhine/IR/Tensor.hpp"

#include <iostream>

namespace rhine {
llvm::Value *Tensor::generate(llvm::Module *M) {
  auto K = context();
  auto ElTy = type()->containedType()->generate(M);
  auto PElTy = llvm::PointerType::get(ElTy, 0);
  auto Dims = type()->getDims();
  auto NElements = 1;
  for (auto Dim : Dims)
    NElements *= Dim;
  auto ConstElts = ConstantInt::get(NElements, 64, K)->generate(M);
  auto ConstPtrSize = ConstantInt::get(8, 64, K)->generate(M);
  auto TensorLen = K->Builder->CreateMul(ConstElts, ConstPtrSize);
  auto MallocF = Externals::get(K)->mappingVal("malloc", M);
  auto Slot = K->Builder->CreateCall(MallocF, {TensorLen}, "TensorAlloc");
  auto CastSlot = K->Builder->CreateBitCast(Slot, PElTy);
  auto PtrN = [K, M, CastSlot](size_t N) {
    auto Idx = ConstantInt::get(N, 32, K)->generate(M);
    return K->Builder->CreateInBoundsGEP(CastSlot, Idx);
  };
  for (auto Idx = 0; Idx < NElements; Idx++) {
    auto ElToStore = getElts()[Idx]->generate(M);
    auto SlotToStoreIn = PtrN(Idx);
    K->Builder->CreateStore(ElToStore, SlotToStoreIn);
  }
  return PtrN(0);
}
}
