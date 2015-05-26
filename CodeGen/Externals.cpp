#include "rhine/IR.h"
#include "rhine/Externals.h"

#include "llvm/IR/Type.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ExecutionEngine/MCJIT.h"

using namespace llvm;

namespace rhine {
Externals::Externals(Context *K) {
  PrintfTy =
    FunctionType::get(IntegerType::get(32, K), {StringType::get(K)}, K);
  MallocTy =
    FunctionType::get(StringType::get(K), {IntegerType::get(64, K)}, K);

  ExternalsMapping.insert(
      std::make_pair("println", ExternalsRef(PrintfTy, printf)));
  ExternalsMapping.insert(
      std::make_pair("malloc", ExternalsRef(MallocTy, malloc)));
}

Externals *Externals::get(Context *K) {
  if (!K->ExternalsCache)
    K->ExternalsCache = new (K->RhAllocator) Externals(K);
  return K->ExternalsCache;
}

FunctionType *Externals::getMappingTy(std::string S) {
  auto V = ExternalsMapping.find(S);
  return V == ExternalsMapping.end() ? nullptr : V->second.FTy;
}

ExternalsFTy *Externals::getMappingVal(std::string S) {
  auto V = ExternalsMapping.find(S);
  return V == ExternalsMapping.end() ? nullptr : V->second.FGenerator;
}

llvm::Constant *Externals::printf(llvm::Module *M, Context *K) {
  auto ArgTys = llvm::ArrayRef<llvm::Type *>(K->Builder->getInt8PtrTy());
  llvm::FunctionType *printf_type =
    llvm::FunctionType::get(K->Builder->getInt32Ty(), ArgTys, true);

  // getOrInsertFunction::
  //
  // Look up the specified function in the module symbol table.
  // Four possibilities: 1. If it does not exist, add a prototype for the function
  // and return it. 2. If it exists, and has a local linkage, the existing
  // function is renamed and a new one is inserted. 3. Otherwise, if the existing
  // function has the correct prototype, return the existing function. 4. Finally,
  // the function exists but has the wrong prototype: return the function with a
  // constantexpr cast to the right prototype.
  return M->getOrInsertFunction("printf", printf_type);
}

llvm::Constant *Externals::malloc(llvm::Module *M, Context *K) {
  auto ArgTys = llvm::ArrayRef<llvm::Type *>(K->Builder->getInt64Ty());
  llvm::FunctionType *malloc_type =
    llvm::FunctionType::get(K->Builder->getInt8PtrTy(), ArgTys, false);
  return M->getOrInsertFunction("malloc", malloc_type);
}
}
