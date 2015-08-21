#include "rhine/Context.h"
#include "rhine/IR/BasicBlock.h"
#include "rhine/IR/Constant.h"
#include "rhine/IR/Module.h"
#include "rhine/IR/Instruction.h"
#include "rhine/IR/UnresolvedValue.h"
#include "rhine/Transform/ResolveLocals.h"
#include "rhine/Externals.h"

namespace rhine {
ResolveLocals::ResolveLocals() : K(nullptr) {}

ResolveLocals::~ResolveLocals() {}

void ResolveLocals::lookupReplaceUse(UnresolvedValue *V, Use &U,
                                     BasicBlock *Block) {
  auto Name = V->getName();
  auto K = V->getContext();
  if (auto S = K->Map.get(V, Block)) {
    if (isa<MallocInst>(S) || isa<Function>(S)) {
      auto Replacement = LoadInst::get(Name, UnType::get(K));
      Replacement->setSourceLocation(V->getSourceLocation());
      U.set(Replacement);
    }
    else if (isa<Argument>(S)) {
      U.set(S);
    } else if (isa<Prototype>(S)) {
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
    if (auto R = dyn_cast<UnresolvedValue>(V))
      lookupReplaceUse(R, ThisUse, BB);
  }
}

void ResolveLocals::runOnFunction(Function *F) {
  for (auto &Arg : F->args())
    K->Map.add(Arg, F->getEntryBlock());
  for (auto &V : *F) {
    if (auto M = dyn_cast<MallocInst>(V)) {
      K->Map.add(M, F->getEntryBlock());
    }
  }
  for (auto &V : *F)
    resolveOperandsOfUser(cast<User>(V), F->getEntryBlock());
}

void ResolveLocals::runOnModule(Module *M) {
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
