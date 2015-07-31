#include "rhine/Context.h"
#include "rhine/IR/Value.h"
#include "rhine/Externals.h"

namespace rhine {
llvm::Value *Argument::toLL(llvm::Module *M, Context *K) {
  auto Name = getName();
  if (auto Result = K->getMappingVal(Name))
    return Result;
  else if (auto Result = Externals::get(K)->getMappingVal(Name, M))
    return Result;
  K->DiagPrinter->errorReport(
      SourceLoc, "unbound argument " + Name);
  exit(1);
}

llvm::Value *GlobalString::toLL(llvm::Module *M, Context *K) {
  // Returns GEP to GlobalStringPtr, which is a Value; data itself is in
  // constant storage.
  auto SRef = llvm::StringRef(getVal());
  return K->Builder->CreateGlobalStringPtr(SRef);
}
}
