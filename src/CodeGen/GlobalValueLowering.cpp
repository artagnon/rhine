#include "rhine/IR/GlobalValue.hpp"
#include "rhine/Diagnostic/Diagnostic.hpp"
#include "rhine/Externals.hpp"
#include "rhine/IR/Context.hpp"

namespace rhine {
llvm::Value *GlobalString::generate(llvm::Module *M) {
  // Returns GEP to GlobalStringPtr, which is a Value; data itself is in
  // constant storage.
  auto K = context();
  return K->Builder->CreateGlobalStringPtr(llvm::StringRef(val()));
}
}
