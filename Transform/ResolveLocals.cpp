#include "rhine/Context.h"
#include "rhine/IR/BasicBlock.h"
#include "rhine/IR/Constant.h"
#include "rhine/IR/Value.h"
#include "rhine/Transform/ResolveLocals.h"
#include "rhine/Externals.h"

namespace rhine {
ResolveLocals::ResolveLocals() : K(nullptr) {}

ResolveLocals::~ResolveLocals() {}

void ResolveLocals::lookupReplaceUse(std::string Name, Use &U,
                                     BasicBlock *Block) {
  if (auto S = lookupNameinBlock(Name, Block)) {
    if (isa<MallocInst>(S) || isa<Function>(S)) {
      auto Replacement = LoadInst::get(Name, UnType::get(K));
      U.set(Replacement);
    }
    else if (isa<Argument>(S)) {
      U.set(S);
    }
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
  K = M->getContext();
  K->GlobalBBHandle = BasicBlock::get({}, K);
  for (auto &F : *M)
    K->Map.addMapping(F->getName(), K->GlobalBBHandle, F);
  for (auto &F : *M)
    runOnFunction(F);
}

Value *ResolveLocals::lookupNameinBlock(std::string Name, BasicBlock *BB) {
  if (auto Resolution = K->Map.getMapping(Name, BB))
    return Resolution->Val;
  if (auto Resolution = K->Map.getMapping(Name, K->GlobalBBHandle))
    return Resolution->Val;
  return nullptr;
}
}
