#include "rhine/IR/Type.hpp"
#include "rhine/Externals.hpp"
#include "rhine/IR/Context.hpp"
#include "rhine/IR/Function.hpp"

#include "llvm/IR/Type.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ExecutionEngine/MCJIT.h"

#define THIS_FPTR(FPtr) ((this)->*(FPtr))

using namespace llvm;

namespace rhine {
Externals::Externals(Context *K) {
  auto PrintTy =
    FunctionType::get(VoidType::get(K), {StringType::get(K)}, true);
  auto MallocTy =
    FunctionType::get(StringType::get(K), {IntegerType::get(64, K)}, false);
  auto ToStringTy =
    FunctionType::get(StringType::get(K), {IntegerType::get(32, K)}, false);
  PrintProto = Prototype::get("print", PrintTy);
  PrintlnProto = Prototype::get("println", PrintTy);
  MallocProto = Prototype::get("malloc", MallocTy);
  ToStringProto = Prototype::get("toString", ToStringTy);
}

Externals::~Externals() {
  delete PrintProto;
  delete PrintlnProto;
  delete MallocProto;
  delete ToStringProto;
}

Externals *Externals::get(Context *K) {
  if (!K->ExternalsCache.get())
    K->ExternalsCache.reset(new Externals(K));
  return K->ExternalsCache.get();
}

std::vector<Prototype *> Externals::getProtos() const {
  return { PrintProto, PrintlnProto, MallocProto, ToStringProto };
}

Prototype *Externals::getMappingProto(std::string S) {
  if (S == "print") return PrintProto;
  else if (S == "println") return PrintlnProto;
  else if (S == "malloc") return MallocProto;
  else if (S == "toString") return ToStringProto;
  return nullptr;
}

llvm::Constant *Externals::mappingVal(std::string S, llvm::Module *M) {
  if (auto Proto = getMappingProto(S)) {
    auto FTy = cast<llvm::FunctionType>(Proto->type()->generate(M));
    return M->getOrInsertFunction(Proto->getMangledName(), FTy);
  }
  return nullptr;
}

extern "C" {
  __attribute__((used, noinline))
  const void *std_String_malloc__Int(int Size) {
    return malloc(Size);
  }
  __attribute__((used, noinline))
  const char *std_String_toString__Int(int ToConvert) {
    auto newString = new std::string(std::to_string(ToConvert));
    return newString->c_str();
  }
  __attribute__((used, noinline))
  void std_Void_print__String(const char *ToPrint) {
    std::cout << ToPrint;
  }
  __attribute__((used, noinline))
  void std_Void_println__String(const char *ToPrint) {
    std::cout << ToPrint << std::endl;
  }
}
}
