#include "rhine/IR.h"
#include "rhine/Transform/LambdaLifting.h"

namespace rhine {
void LambdaLifting::runOnFunction(Function *F) {
  auto M = F->getParent();
  std::transform(
      F->begin(), F->end(), F->begin(),
      [this, F, M](Value *V) {
        if (auto B = dyn_cast<MallocInst>(V)) {
          if (auto Fn = dyn_cast<Function>(B->getVal())) {
            Fn->setName("lambda");
            auto It = std::find(M->begin(), M->end(), F);
            assert(It != M->end() && "Function parent not set");
            M->insertFunction(It, Fn);
            auto K = F->getContext();
            auto Sym = LoadInst::get(Fn->getName(), UnType::get(K), K);
            B->setVal(Sym);
          }
        }
        return V;
      });
}
}
