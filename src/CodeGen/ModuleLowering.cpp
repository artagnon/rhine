#include "rhine/IR/Context.h"
#include "rhine/IR/Module.h"
#include "rhine/Externals.h"

namespace rhine {
void Module::toLL(llvm::Module *M) {
  for (auto F: getVal())
    F->toLL(M);
}
}
