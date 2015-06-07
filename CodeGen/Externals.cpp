#include "rhine/IR.h"
#include "rhine/Externals.h"

#include "llvm/IR/Type.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ExecutionEngine/MCJIT.h"

using namespace llvm;

namespace rhine {
FunctionType *Externals::PrintfTy;
FunctionType *Externals::MallocTy;
FunctionType *Externals::ToStringTy;

Externals::Externals(Context *K) {
  PrintfTy =
    FunctionType::get(IntegerType::get(32, K), {StringType::get(K)}, true, K);
  MallocTy =
    FunctionType::get(StringType::get(K), {IntegerType::get(64, K)}, false, K);
  ToStringTy =
    FunctionType::get(StringType::get(K), {IntegerType::get(32, K)}, false, K);
  auto PrintfTyPtr = PointerType::get(PrintfTy, K);
  auto MallocTyPtr = PointerType::get(MallocTy, K);
  auto ToStringTyPtr = PointerType::get(ToStringTy, K);

  ExternalsMapping.insert(
      std::make_pair("println", ExternalsRef(PrintfTyPtr, printf)));
  ExternalsMapping.insert(
      std::make_pair("malloc", ExternalsRef(MallocTyPtr, malloc)));
  ExternalsMapping.insert(
      std::make_pair("toString", ExternalsRef(ToStringTyPtr, toString)));
}

Externals *Externals::get(Context *K) {
  if (!K->ExternalsCache)
    K->ExternalsCache = new (K->RhAllocator) Externals(K);
  return K->ExternalsCache;
}

PointerType *Externals::getMappingTy(std::string S) {
  auto V = ExternalsMapping.find(S);
  return V == ExternalsMapping.end() ? nullptr : V->second.FTy;
}

ExternalsFTy *Externals::getMappingVal(std::string S) {
  auto V = ExternalsMapping.find(S);
  return V == ExternalsMapping.end() ? nullptr : V->second.FGenerator;
}

llvm::Constant *Externals::printf(llvm::Module *M, Context *K) {
  // getOrInsertFunction::
  //
  // Look up the specified function in the module symbol table.
  // Four possibilities: 1. If it does not exist, add a prototype for the function
  // and return it. 2. If it exists, and has a local linkage, the existing
  // function is renamed and a new one is inserted. 3. Otherwise, if the existing
  // function has the correct prototype, return the existing function. 4. Finally,
  // the function exists but has the wrong prototype: return the function with a
  // constantexpr cast to the right prototype.
  auto FTy = cast<llvm::FunctionType>(PrintfTy->toLL(M, K));
  return M->getOrInsertFunction("printf", FTy);
}

llvm::Constant *Externals::malloc(llvm::Module *M, Context *K) {
  auto FTy = cast<llvm::FunctionType>(MallocTy->toLL(M, K));
  return M->getOrInsertFunction("malloc", FTy);
}

llvm::Constant *Externals::toString(llvm::Module *M, Context *K) {
  auto FTy = cast<llvm::FunctionType>(ToStringTy->toLL(M, K));
  return M->getOrInsertFunction("std_toString_int", FTy);
}


//===----------------------------------------------------------------------===//
//
// Intrinsics are declared here because they get blown away during link stage if
// they're in another file.
//
//===----------------------------------------------------------------------===//
extern "C" {
  __attribute__((used, noinline))
  const char *std_toString_int(int toConvert) {
    return std::to_string(toConvert).c_str();
  }
}
}
