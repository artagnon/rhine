#include "rhine/IR/Context.h"
#include "rhine/IR/Value.h"
#include "rhine/IR/UnresolvedValue.h"
#include "rhine/IR/GlobalValue.h"
#include "rhine/Externals.h"

namespace rhine {
llvm::Value *UnresolvedValue::toLL(llvm::Module *M) {
  auto K = getContext();
  K->DiagPrinter->errorReport(
      SourceLoc, "ResolveValues not run properly" + Name);
  exit(1);
}

llvm::Value *Argument::toLL(llvm::Module *M) {
  auto K = getContext();
  auto Name = getName();
  if (auto Result = K->Map.getl(this))
    return Result;
  else if (auto Result = Externals::get(K)->getMappingVal(Name, M))
    return Result;
  K->DiagPrinter->errorReport(
      SourceLoc, "unbound argument " + Name);
  exit(1);
}

llvm::Value *GlobalString::toLL(llvm::Module *M) {
  // Returns GEP to GlobalStringPtr, which is a Value; data itself is in
  // constant storage.
  auto K = getContext();
  return K->Builder->CreateGlobalStringPtr(llvm::StringRef(getVal()));
}
}
