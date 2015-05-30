#include "rhine/IR.h"
#include "rhine/LambdaLifting.h"

namespace rhine {
void LambdaLifting::runOnFunction(Function *F) {
  auto M = F->getParent();
  std::transform(
      F->begin(), F->end(), F->begin(),
      [this, F, M](Value *V) {
        if (auto B = dyn_cast<BindInst>(V)) {
          if (auto Fn = dyn_cast<Function>(B->getVal())) {
            Fn->setName("lambda");
            auto It = std::find(M->begin(), M->end(), F);
            assert(It != M->end() && "Function parent not set");
            M->insertFunction(It, Fn);
            auto Sym = Symbol::get(Fn->getName(), UnType::get(K), K);
            B->setVal(Sym);
          }
        }
        return V;
      });
}

void LambdaLifting::runOnModule(Module *M) {
  for (auto F: *M)
    runOnFunction(F);
}
}
