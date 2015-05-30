#include "rhine/IR.h"
#include "rhine/LambdaLifting.h"

namespace rhine {
Function *LambdaLifting::functionFromLambda(Lambda *L) {
  auto NewF = Function::get(cast<FunctionType>(L->getType()), K);
  NewF->setName("lambda");
  NewF->setBody(L->getVal());
  return NewF;
}

void LambdaLifting::runOnFunction(Function *F) {
  auto M = F->getParent();
  std::transform(
      F->begin(), F->end(), F->begin(),
      [this, F, M](Value *V) {
        if (auto B = dyn_cast<BindInst>(V)) {
          if (auto L = dyn_cast<Lambda>(B->getVal())) {
            auto NewF = functionFromLambda(L);
            auto It = std::find(M->begin(), M->end(), F);
            assert(It != M->end() && "Function parent not set");
            M->insertFunction(It, NewF);
            auto Sym = Symbol::get(NewF->getName(), UnType::get(K), K);
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
