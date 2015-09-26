#include "rhine/Context.h"
#include "rhine/IR/BasicBlock.h"
#include "rhine/IR/Instruction.h"
#include "rhine/IR/UnresolvedValue.h"
#include "rhine/IR/Module.h"
#include "rhine/Transform/Resolve.h"
#include "rhine/Externals.h"

#include <list>

namespace rhine {
Resolve::Resolve() : K(nullptr) {}

Resolve::~Resolve() {}

void Resolve::lookupReplaceUse(UnresolvedValue *V, Use &U,
                               BasicBlock *Block) {
  auto Name = V->getName();
  auto K = V->getContext();
  if (auto S = K->Map.get(V, Block)) {
    if (auto M = dyn_cast<MallocInst>(S)) {
      auto Replacement = LoadInst::get(M);
      Replacement->setSourceLocation(V->getSourceLocation());
      U.set(Replacement);
    }
    else if (isa<Argument>(S)) {
      U.set(S);
    } else if (isa<Prototype>(S)) {
      auto Replacement = Pointer::get(S);
      Replacement->setSourceLocation(S->getSourceLocation());
      U.set(Replacement);
    }
  } else {
    auto SourceLoc = U->getSourceLocation();
    auto K = Block->getContext();
    switch (U->getUser()->getValID()) {
    case RT_CallInst:
      K->DiagPrinter->errorReport(SourceLoc, "unbound function " + Name);
      exit(1);
    default:
      K->DiagPrinter->errorReport(SourceLoc, "unbound symbol " + Name);
      exit(1);
    }
  }
}

void Resolve::resolveOperandsOfUser(User *U, BasicBlock *BB) {
  for (Use &ThisUse : U->uses()) {
    Value *V = ThisUse;
    if (auto R = dyn_cast<UnresolvedValue>(V))
      lookupReplaceUse(R, ThisUse, BB);

    if (auto W = dyn_cast<User>(V))
      resolveOperandsOfUser(W, BB);
  }
}

void Resolve::runOnFunction(Function *F) {
  for (auto &Arg : F->args())
    K->Map.add(Arg, F->getEntryBlock());

  for (auto &BB : *F)
    for (auto It = BB->begin(); It != BB->end(); ++It) {
      auto &V = *It;
      if (auto M = dyn_cast<MallocInst>(V)) {
        if (auto OldValue = K->Map.get(V, BB)) {
          auto NewInst = StoreInst::get(OldValue, M->getVal());
          std::replace(It, It + 1, V, cast<Value>(NewInst));
        } else
          K->Map.add(M, BB);
      }
    }

  for (auto &BB : *F)
    for (auto &V : *BB)
      resolveOperandsOfUser(cast<User>(V), BB);
}

void Resolve::runOnModule(Module *M) {
  K = M->getContext();
  for (auto P : Externals::get(K)->getProtos())
    K->Map.add(P);
  for (auto &F : *M)
    K->Map.add(F);
  for (auto &F : *M)
    runOnFunction(F);
}

using KR = Context::ResolutionMap;

void KR::add(Value *Val, BasicBlock *Block) {
  assert(!isa<UnresolvedValue>(Val));
  auto &ThisResolutionMap = BlockResolutionMap[Block];
  auto Ret = ThisResolutionMap.insert(std::make_pair(Val->getName(), Val));
  bool NewElementInserted = Ret.second;
  if (!NewElementInserted) {
    auto IteratorToEquivalentKey = Ret.first;
    auto &ValueOfEquivalentKey = IteratorToEquivalentKey->second;
    assert(ValueOfEquivalentKey == Val &&
           "Inserting conflicting values into ResolutionMap");
  }
}

void KR::add(Value *Val, llvm::Value *LLVal) {
  assert(!isa<UnresolvedValue>(Val));
  auto Ret = LoweringMap.insert(std::make_pair(Val, LLVal));
  bool NewElementInserted = Ret.second;
  if (!NewElementInserted) {
    auto IteratorToEquivalentKey = Ret.first;
    auto &ValueOfEquivalentKey = IteratorToEquivalentKey->second;
    assert(ValueOfEquivalentKey == LLVal &&
           "Inserting conflicting values into LoweringMap");
  }
}

Value *KR::searchOneBlock(Value *Val, BasicBlock *Block) {
  auto Name = Val->getName();
  auto &ThisResolutionMap = BlockResolutionMap[Block];
  auto IteratorToElement = ThisResolutionMap.find(Name);
  if (IteratorToElement == ThisResolutionMap.end())
    return nullptr;
  return IteratorToElement->second;
}

void flattenPredecessors(BasicBlock *Block, std::list<BasicBlock *> &AllPreds) {
  if (!Block) return;
  AllPreds.push_back(Block);
  for (auto Pred : Block->preds())
    flattenPredecessors(Pred, AllPreds);
}

Value *KR::get(Value *Val, BasicBlock *Block) {
  std::list<BasicBlock *> UniqPreds;
  flattenPredecessors(Block, UniqPreds);
  UniqPreds.unique();
  UniqPreds.push_back(nullptr); // Global
  for (auto BB : UniqPreds)
    if (auto Result = searchOneBlock(Val, BB))
      return Result;
  return nullptr;
}

llvm::Value *KR::getl(Value *Val) {
  auto IteratorToElement = LoweringMap.find(Val);
  return IteratorToElement == LoweringMap.end() ? nullptr :
    IteratorToElement->second;
}
}
