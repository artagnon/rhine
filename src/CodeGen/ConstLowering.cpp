#include "rhine/Context.h"
#include "rhine/IR/Constant.h"
#include "rhine/IR/Instruction.h"
#include "rhine/Externals.h"

namespace rhine {
llvm::Constant *ConstantInt::toLL(llvm::Module *M) {
  auto LLTy = getType()->toLL(M);
  return llvm::ConstantInt::get(LLTy, getVal());
}

llvm::Constant *ConstantBool::toLL(llvm::Module *M) {
  auto LLTy = getType()->toLL(M);
  return llvm::ConstantInt::get(LLTy, getVal());
}

llvm::Constant *ConstantFloat::toLL(llvm::Module *M) {
  auto LLTy = getType()->toLL(M);
  return llvm::ConstantFP::get(LLTy, getVal());
}

llvm::Function *Prototype::getOrInsert(llvm::Module *M) {
  auto K = getContext();
  auto FnTy = cast<llvm::FunctionType>(getType()->toLL(M));
  auto MangledName = getMangledName();

  // getOrInsertFunction::
  //
  // Look up the specified function in the module symbol table.
  // Four possibilities: 1. If it does not exist, add a prototype for the function
  // and return it. 2. If it exists, and has a local linkage, the existing
  // function is renamed and a new one is inserted. 3. Otherwise, if the existing
  // function has the correct prototype, return the existing function. 4. Finally,
  // the function exists but has the wrong prototype: return the function with a
  // constantexpr cast to the right prototype.
  auto Const = M->getOrInsertFunction(MangledName, FnTy);

  if (auto FunctionCandidate = dyn_cast<llvm::Function>(Const))
    return FunctionCandidate;
  auto Error = MangledName + " was declared with different signature earlier";
  K->DiagPrinter->errorReport(SourceLoc, Error);
  exit(1);
}

llvm::Constant *Prototype::toLL(llvm::Module *M) {
  return getOrInsert(M);
}

llvm::Constant *Function::toLL(llvm::Module *M) {
  auto K = getContext();
  auto CurrentFunction = getOrInsert(M);
  CurrentFunction->setGC("rhgc");

  // Bind argument symbols to function argument values in symbol table
  auto ArgList = getArguments();
  auto S = ArgList.begin();
  for (auto &Arg : CurrentFunction->args()) {
    K->Map.add(*S, &Arg);
    ++S;
  }

  // Add function symbol to symbol table, global scope
  K->Map.add(this, CurrentFunction);

  getEntryBlock()->toLL(M);
  auto ExitBlock = getExitBlock();
  if (!ExitBlock->size() || !isa<ReturnInst>(ExitBlock->back()))
    K->Builder->CreateRet(nullptr);
  return CurrentFunction;
}

llvm::Value *Pointer::toLL(llvm::Module *M) {
  auto K = getContext();
  auto Name = getVal()->getName();
  if (auto Val = K->Map.getl(getVal()))
    return Val;
  return Externals::get(K)->getMappingVal(Name, M);
}
}
