#include "rhine/Context.h"
#include "rhine/IR/BasicBlock.h"
#include "rhine/IR/Constant.h"
#include "rhine/IR/Module.h"
#include "rhine/IR/Instruction.h"
#include "rhine/IR/UnresolvedValue.h"
#include "rhine/Transform/Resolve.h"
#include "rhine/Externals.h"

namespace rhine {
Resolve::Resolve() : K(nullptr) {}

Resolve::~Resolve() {}

void Resolve::lookupReplaceUse(UnresolvedValue *V, Use &U,
                                     BasicBlock *Block) {
  auto Name = V->getName();
  auto K = V->getContext();
  if (auto S = K->Map.get(V, Block)) {
    if (auto M = dyn_cast<MallocInst>(S)) {
      auto Replacement = LoadInst::get(M->getVal(), Name);
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

  for (auto &V : *F->front())
    if (auto M = dyn_cast<MallocInst>(V))
      K->Map.add(M, F->getEntryBlock());

  for (auto &V : *F->front())
    resolveOperandsOfUser(cast<User>(V), F->getEntryBlock());
}

void Resolve::runOnModule(Module *M) {
  K = M->getContext();
  for (auto &F : *M)
    K->Map.add(F);
  for (auto &F : *M)
    runOnFunction(F);
}

using KR = Context::ResolutionMap;
typedef Context::ValueRef ValueRef;

void KR::add(Value *Val, BasicBlock *Block, llvm::Value *LLVal) {
  assert(!isa<UnresolvedValue>(Val));
  auto Name = Val->getName();
  auto &ThisVRMap = FunctionVR[Block];
  auto Ret = ThisVRMap.insert(std::make_pair(Name, ValueRef(Val, LLVal)));
  auto &NewElementInserted = Ret.second;
  if (!NewElementInserted) {
    auto IteratorToEquivalentKey = Ret.first;
    auto &ValueRefOfEquivalentKey = IteratorToEquivalentKey->second;
    ValueRefOfEquivalentKey.Val = Val;
    if (LLVal) ValueRefOfEquivalentKey.LLVal = LLVal;
  }
}

Value *KR::searchOneBlock(Value *Val, BasicBlock *Block)
{
  auto Name = Val->getName();
  auto &ThisVRMap = FunctionVR[Block];
  auto IteratorToElement = ThisVRMap.find(Name);
  if (IteratorToElement == ThisVRMap.end())
    return nullptr;
  else
    return IteratorToElement->second.Val;
}

Value *KR::get(Value *Val, BasicBlock *Block) {
  if (!Val->isUnTyped() && !isa<UnresolvedValue>(Val))
    return Val;
  for (; Block; Block = Block->getPredecessor()) {
    if (auto Result = searchOneBlock(Val, Block))
      return Result;
  }
  if (auto Result = searchOneBlock(Val, nullptr))
    return Result;
  auto Ext = Externals::get(Val->getContext());
  return Ext->getMappingProto(Val->getName());
}

llvm::Value *KR::getl(Value *Val, BasicBlock *Block) {
  auto Name = Val->getName();
  auto &ThisVRMap = FunctionVR[Block];
  auto IteratorToElement = ThisVRMap.find(Name);
  return IteratorToElement == ThisVRMap.end() ? nullptr :
    IteratorToElement->second.LLVal;
}
}
