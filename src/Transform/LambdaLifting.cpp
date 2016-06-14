#include "rhine/Transform/LambdaLifting.hpp"
#include "rhine/IR/Constant.hpp"
#include "rhine/IR/Instruction.hpp"
#include "rhine/IR/Module.hpp"
#include "rhine/IR/UnresolvedValue.hpp"

namespace rhine {
void LambdaLifting::runOnFunction(Function *F) {
  auto M = F->getParent();
  for (auto &BB : *F) {
    for (auto &V : *BB) {
      if (auto B = dyn_cast<MallocInst>(V)) {
        if (auto Fn = dyn_cast<Function>(B->val())) {
          Fn->setName("lambda");
          auto It = std::find(M->begin(), M->end(), F);
          assert(It != M->end() && "Function parent not set");
          M->insertFunction(It, Fn);
          auto K = F->getContext();
          auto Sym = UnresolvedValue::get(Fn->getName(), UnType::get(K));
          B->setVal(Sym);
        }
      }
    }
  }
}
}
