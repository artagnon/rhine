#include "rhine/Context.h"
#include "rhine/IR/Value.h"

namespace rhine {
llvm::Value *Symbol::toLL(llvm::Module *M, Context *K) {
  assert(K && "null Symbol Table");
  return K->getMappingVal(getName(), getSourceLocation());
}

llvm::Value *GlobalString::toLL(llvm::Module *M, Context *K) {
  // Returns GEP to GlobalStringPtr, which is a Value; data itself is in
  // constant storage.
  auto SRef = llvm::StringRef(getVal());
  return K->Builder->CreateGlobalStringPtr(SRef);
}
}
