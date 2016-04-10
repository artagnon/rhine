#include "rhine/IR/UnresolvedValue.hpp"
#include "rhine/Diagnostic/Diagnostic.hpp"
#include "rhine/Externals.hpp"
#include "rhine/IR/Context.hpp"

namespace rhine {
llvm::Value *UnresolvedValue::toLL(llvm::Module *M) {
  DiagnosticPrinter(SourceLoc) << "ResolveValues not run properly" + Name;
  exit(1);
}

llvm::Value *Argument::toLL(llvm::Module *M) {
  if (LoweredValue) {
    return LoweredValue;
  }
  auto K = getContext();
  auto Name = getName();
  if (auto Result = Externals::get(K)->getMappingVal(Name, M)) {
    LoweredValue = Result;
    return Result;
  }
  DiagnosticPrinter(SourceLoc) << "unbound argument " + Name;
  exit(1);
}
}
