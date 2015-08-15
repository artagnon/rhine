#include "rhine/Context.h"
#include "rhine/IR/BasicBlock.h"
#include "rhine/IR/Constant.h"
#include "rhine/IR/Value.h"
#include "rhine/Transform/ResolveLocals.h"

namespace rhine {
void ResolveLocals::lookupReplaceUse(std::string Name, Use &U,
                                     BasicBlock *Block) {
  if (auto S = lookupNameinBlock(Name, Block)) {
    auto L = LoadInst::get(Name, S->getType());
    U.set(L);
  } else {
    auto SourceLoc = U->getSourceLocation();
    auto K = Block->getContext();
    K->DiagPrinter->errorReport(SourceLoc, "unbound symbol " + Name);
    exit(1);
  }
}

void ResolveLocals::resolveOperandsOfUser(User *U, BasicBlock *BB) {
  for (Use &ThisUse : U->operands()) {
    Value *V = ThisUse;
    if (auto W = dyn_cast<User>(V))
      resolveOperandsOfUser(W, BB);
    auto Name = V->getName();
    if (isa<UnresolvedValue>(V))
      lookupReplaceUse(Name, ThisUse, BB);
  }
}

void ResolveLocals::runOnFunction(Function *F) {
  K = F->getContext();
  for (auto &Arg : F->args())
    K->Map.addMapping(Arg->getName(), F->getEntryBlock(), Arg);
  for (auto &V : *F) {
    if (auto M = dyn_cast<MallocInst>(V)) {
      K->Map.addMapping(M->getName(), F->getEntryBlock(), M);
    }
  }
  for (auto &V : *F)
    resolveOperandsOfUser(cast<User>(V), F->getEntryBlock());
}

void ResolveLocals::runOnModule(Module *M) {
  // for (auto &F : *M)
  //   K->Map.addMapping(F->getName(), K->GlobalBBHandle, F);
  for (auto &F : *M)
    runOnFunction(F);
}

Value *ResolveLocals::lookupNameinBlock(std::string Name, BasicBlock *BB) {
  if (auto Resolution = K->Map.getMapping(Name, BB))
    return Resolution->Val;
  return nullptr;
}
}
