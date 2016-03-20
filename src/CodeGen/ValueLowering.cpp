#include "rhine/IR/Context.hpp"
#include "rhine/IR/UnresolvedValue.hpp"
#include "rhine/IR/GlobalValue.hpp"
#include "rhine/Externals.hpp"
#include "rhine/Diagnostic/Diagnostic.hpp"

namespace rhine {
llvm::Value *UnresolvedValue::toLL(llvm::Module *M) {
  DiagnosticPrinter(SourceLoc) << "ResolveValues not run properly" + Name;
  exit(1);
}

llvm::Value *Argument::toLL(llvm::Module *M) {
  auto K = getContext();
  auto Name = getName();
  if (auto Result = K->Map.getl(this))
    return Result;
  else if (auto Result = Externals::get(K)->getMappingVal(Name, M))
    return Result;
  DiagnosticPrinter(SourceLoc) << "unbound argument " + Name;
  exit(1);
}

llvm::Value *GlobalString::toLL(llvm::Module *M) {
  // Returns GEP to GlobalStringPtr, which is a Value; data itself is in
  // constant storage.
  auto K = getContext();
  return K->Builder->CreateGlobalStringPtr(llvm::StringRef(getVal()));
}
}
