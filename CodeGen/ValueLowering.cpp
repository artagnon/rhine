#include "rhine/Context.h"
#include "rhine/IR/Value.h"

namespace rhine {
llvm::Value *Symbol::toLL(llvm::Module *M, Context *K) {
  assert(K && "null Symbol Table");
  return K->getMapping(getName(), getSourceLocation());
}

llvm::Value *GlobalString::toLL(llvm::Module *M, Context *K) {
  auto SRef = llvm::StringRef(getVal());
  return K->Builder->CreateGlobalStringPtr(SRef);
}
}
