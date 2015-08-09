#include "rhine/IR/Module.h"

namespace rhine {
Module::Module(Context *K) : Kontext(K) {}

Module *Module::get(Context *K) {
  return new (K->RhAllocator) Module(K);
}

Function *Module::front() {
  return ContainedFs.front();
}

Module::iterator Module::begin() {
  return ContainedFs.begin();
}

Module::iterator Module::end() {
  return ContainedFs.end();
}
}
