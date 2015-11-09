#include "rhine/Diagnostic/Diagnostic.h"
#include "rhine/IR/BasicBlock.h"
#include "rhine/IR/Context.h"
#include "rhine/Externals.h"

namespace rhine {
Context::Context(std::ostream &ErrStream):
    LLContext(llvm::getGlobalContext()),
    Builder(new llvm::IRBuilder<>(LLContext)),
    DiagPrinter(new DiagnosticPrinter(&ErrStream)),
    ExternalsCache(nullptr) {}

Context::~Context() {
  delete Builder;
  delete DiagPrinter;
  delete ExternalsCache;

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
