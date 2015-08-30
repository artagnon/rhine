#include "rhine/Externals.h"
#include "rhine/IR/Constant.h"
#include "rhine/IR/Type.h"

#include "llvm/IR/Type.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/ExecutionEngine/MCJIT.h"

#define THIS_FPTR(FPtr) ((this)->*(FPtr))

using namespace llvm;

namespace rhine {
Externals::Externals(Context *K) {
  auto PrintTy =
    FunctionType::get(VoidType::get(K), {StringType::get(K)}, true, K);
  auto MallocTy =
    FunctionType::get(StringType::get(K), {IntegerType::get(64, K)}, false, K);
  auto ToStringTy =
    FunctionType::get(StringType::get(K), {IntegerType::get(32, K)}, false, K);
  PrintProto = Prototype::get("print", PrintTy);
  PrintlnProto = Prototype::get("println", PrintTy);
  MallocProto = Prototype::get("malloc", MallocTy);
  ToStringProto = Prototype::get("toString", ToStringTy);
}

Externals *Externals::get(Context *K) {
  if (!K->ExternalsCache)
    K->ExternalsCache = new (K->RhAllocator) Externals(K);
  return K->ExternalsCache;
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

llvm::Constant *Externals::getMappingVal(std::string S, llvm::Module *M) {
  if (auto Proto = getMappingProto(S)) {
    auto FTy = cast<llvm::FunctionType>(Proto->getType()->toLL(M));
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
