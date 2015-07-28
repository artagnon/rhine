#include "rhine/IR.h"
#include "rhine/Transform/FunctionTransform.h"

namespace rhine {
void FunctionTransform::runOnModule(Module *M) {
  auto V = M->getVal();
  std::transform(V.begin(), V.end(), V.begin(),
                 [this](Function *F) -> Function * {
                   runOnFunction(F);
                   return F;
                 });
}
}
