#include "rhine/IR.h"
#include "rhine/Transform/FunctionPass.h"

namespace rhine {
void FunctionPass::runOnModule(Module *M) {
  auto V = M->getVal();
  std::transform(V.begin(), V.end(), V.begin(),
                 [this](Function *F) -> Function * {
                   runOnFunction(F);
                   return F;
                 });
}
}
