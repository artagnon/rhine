#include "rhine/IR/Function.hpp"
#include "rhine/Diagnostic/Diagnostic.hpp"
#include "rhine/Externals.hpp"
#include "rhine/IR/Context.hpp"
#include "rhine/IR/Instruction.hpp"

namespace rhine {
llvm::Function *Prototype::getOrInsert(llvm::Module *M) {
  auto FnTy = cast<llvm::FunctionType>(type()->toLL(M));
  auto MangledName = getMangledName();

  /// getOrInsertFunction::
  ///
  /// Look up the specified function in the module symbol table.
  /// Four possibilities:
  /// 1. If it does not exist, add a prototype for the function and return it.
  /// 2. If it exists, and has a local linkage, the existing function is
  /// renamed and a new one is inserted.
  /// 3. Otherwise, if the existing function has the correct prototype, return
  /// the existing function.
  /// 4. Finally, the function exists but has the wrong prototype: return the
  /// function with a constantexpr cast to the right prototype.
  auto Const = M->getOrInsertFunction(MangledName, FnTy);

  if (auto FunctionCandidate = dyn_cast<llvm::Function>(Const))
    return FunctionCandidate;
  auto Error = MangledName + " was declared with different signature earlier";
  DiagnosticPrinter(SourceLoc) << Error;
  exit(1);
}

llvm::Constant *Prototype::toLL(llvm::Module *M) { return getOrInsert(M); }

/// Codegen function prototype and all blocks.
llvm::Constant *Function::toLL(llvm::Module *M) {
  if (LoweredValue) {
    return cast<llvm::Constant>(LoweredValue);
  }
  auto K = context();
  auto CurrentFunction = getOrInsert(M);

  /// Bind argument symbols to function argument values in symbol table
  auto ArgList = getArguments();
  auto S = ArgList.begin();
  for (auto &Arg : CurrentFunction->args()) {
    auto SourceLoc = (*S)->sourceLocation();
    (*S)->setLoweredValue(&Arg);
    ++S;
  }

  // Add function symbol to symbol table, global scope
  setLoweredValue(CurrentFunction);

  /// Codegens all blocks
  getEntryBlock()->toLL(M);

  auto ExitBlock = getExitBlock();
  if (!ExitBlock->size() || !isa<ReturnInst>(ExitBlock->back()))
    K->Builder->CreateRet(nullptr);
  return CurrentFunction;
}
}
