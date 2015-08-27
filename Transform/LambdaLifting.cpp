#include "rhine/IR/UnresolvedValue.h"
#include "rhine/Transform/LambdaLifting.h"

namespace rhine {
class Function;
class MallocInst;

void LambdaLifting::runOnFunction(Function *F) {
  auto M = F->getParent();
  auto BB = F->front();
  std::transform(
      BB->begin(), BB->end(), BB->begin(),
      [this, F, M](Value *V) {
        if (auto B = dyn_cast<MallocInst>(V)) {
          if (auto Fn = dyn_cast<Function>(B->getVal())) {
            Fn->setName("lambda");
            auto It = std::find(M->begin(), M->end(), F);
            assert(It != M->end() && "Function parent not set");
            M->insertFunction(It, Fn);
            auto K = F->getContext();
            auto Sym = UnresolvedValue::get(Fn->getName(), UnType::get(K));
            B->setVal(Sym);
          }
        }
        return V;
      });
}
}
