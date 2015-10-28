#include "rhine/IR/Context.h"
#include "rhine/Externals.h"

namespace rhine {
Context::~Context() {
  delete Builder;
  delete DiagPrinter;
  delete ExternalsCache;
  RhAllocator.Reset();
  FTyCache.clear();
  ITyCache.clear();
}
}
