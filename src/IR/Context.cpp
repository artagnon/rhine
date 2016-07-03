#include "rhine/Diagnostic/Diagnostic.hpp"
#include "rhine/IR/BasicBlock.hpp"
#include "rhine/IR/Context.hpp"
#include "rhine/Externals.hpp"

namespace rhine {
Context::Context(std::ostream &ErrStream):
    LLContext(new llvm::LLVMContext()),
    Builder(new llvm::IRBuilder<>(*LLContext)),
    DiagPrinter(new DiagnosticPrinter(ErrStream)),
    ExternalsCache(nullptr) {}

LLVMContext &Context::llvmContext() {
  return *LLContext;
}

Context::~Context() {
  /// Cannot delete in-place, because we rely on the "next" pointer embedded in
  /// the node to get to the next element.
  std::vector<Type *> ToDelete;
  for (auto &Ty : ITyCache)
    ToDelete.push_back(&Ty);
  for (auto &Ty : PTyCache)
    ToDelete.push_back(&Ty);
  for (auto &Ty : FTyCache)
    ToDelete.push_back(&Ty);
  for (auto *Ty : ToDelete)
    delete Ty;
  ITyCache.clear();
  PTyCache.clear();
  FTyCache.clear();
  ToDelete.clear();
  for (auto *Ty : UTyCache)
    delete Ty;
  UTyCache.clear();

  /// Now getting rid of constants
  std::vector<Value *> ToDeleteV;
  for (auto &C : CIntCache)
    ToDeleteV.push_back(&C);
  for (auto &C : CBoolCache)
    ToDeleteV.push_back(&C);
  for (auto &C : CFltCache)
    ToDeleteV.push_back(&C);
  for (auto &C : PtrCache)
    ToDeleteV.push_back(&C);
  CIntCache.clear();
  CBoolCache.clear();
  CFltCache.clear();
  PtrCache.clear();
  ToDeleteV.clear();
}
}
