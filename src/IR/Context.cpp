#include "rhine/IR/Context.h"
#include "rhine/Externals.h"

namespace rhine {
Context::~Context() {
  delete Builder;
  delete DiagPrinter;
  delete ExternalsCache;
  FTyCache.clear();
  PTyCache.clear();
  ITyCache.clear();
}
}
