#include "rhine/IR/GlobalValue.hpp"
#include "rhine/Diagnostic/Diagnostic.hpp"
#include "rhine/Externals.hpp"
#include "rhine/IR/Context.hpp"

namespace rhine {
llvm::Value *GlobalString::toLL(llvm::Module *M) {
  // Returns GEP to GlobalStringPtr, which is a Value; data itself is in
  // constant storage.
  auto K = getContext();
  return K->Builder->CreateGlobalStringPtr(llvm::StringRef(getVal()));
}
}
