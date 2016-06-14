#include "rhine/IR/Module.hpp"
#include "rhine/IR/Context.hpp"

namespace rhine {
void Module::toLL(llvm::Module *M) {
  for (auto F : val())
    F->toLL(M);
}
}
