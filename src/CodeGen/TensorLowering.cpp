#include "rhine/IR/Context.hpp"
#include "rhine/IR/Tensor.hpp"

namespace rhine {
llvm::Value *Tensor::toLL(llvm::Module *M) {
  auto ElTy = getType()->getCTy()->toLL(M);
  // auto PElTy = llvm::PointerType::get(ElTy, 0);
  auto Dims = getType()->getDims();
  assert(Dims.size() == 1 && "Multi-dimensional tensor not supported yet");
  auto NElements = 0;
  for (auto Dim : Dims)
    NElements += Dim;
  return nullptr;
}
}
