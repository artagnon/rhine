#include "rhine/IR.h"
#include "rhine/Transform/ModuleTransform.h"

namespace rhine {
void ModuleTransform::runOnModule(Module *M) {
  for (auto F: *M)
    runOnFunction(F);
}
}
