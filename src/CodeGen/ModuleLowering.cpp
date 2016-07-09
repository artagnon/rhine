#include "rhine/IR/Module.hpp"
#include "rhine/IR/Context.hpp"

namespace rhine {
void Module::generate(llvm::Module *M) {
  for (auto F : val())
    F->generate(M);
}
}
