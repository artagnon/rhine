#include "rhine/IR/Context.hpp"
#include "rhine/IR/Module.hpp"
#include "rhine/Externals.hpp"

namespace rhine {
void Module::toLL(llvm::Module *M) {
  for (auto F: val())
    F->toLL(M);
}
}
