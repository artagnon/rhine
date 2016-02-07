#include "rhine/IR/Module.hpp"
#include "rhine/Pass/FunctionPass.hpp"

namespace rhine {
void FunctionPass::runOnModule(Module *M) {
  for (auto &F : *M) {
    runOnFunction(F);
  }
}
}
