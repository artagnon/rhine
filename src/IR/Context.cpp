#include "rhine/IR/Context.h"
#include "rhine/Externals.h"

namespace rhine {
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
}
}
