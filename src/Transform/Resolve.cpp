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

void Resolve::lookupReplaceUse(UnresolvedValue *V, Use &U, BasicBlock *Block) {
  auto Name = V->getName();
  auto K = V->getContext();
  if (auto S = K->Map.get(V, Block)) {
    /// %S = 2;
    ///  ^
    /// Came from here (MallocInst, Argument, or Prototype)
    ///
    /// Foo(%S);
    ///      ^
    ///  UnresolvedValue; replace with %Replacement
    if (auto M = dyn_cast<MallocInst>(S)) {
      if (dyn_cast<StoreInst>(U->getUser()))
        U.set(M);
      else {
        auto Replacement = LoadInst::get(M);
        Replacement->setSourceLocation(V->getSourceLocation());
        U.set(Replacement);
      }
    } else if (isa<Argument>(S)) {
      U.set(S);
    } else if (isa<Prototype>(S)) {
      auto Replacement = Pointer::get(S);
      Replacement->setSourceLocation(S->getSourceLocation());
      U.set(Replacement);
    }
  } else {
    /// %V was not seen earlier (%S not initialized)
    /// Only one possibility: %V(...)
    ///                        ^
    ///                Callee of CallInst
    auto SourceLoc = U->getSourceLocation();
    auto K = Block->getContext();
    if (auto Inst = dyn_cast<CallInst>(U->getUser()))
      if (Inst->getCallee() == V) {
        K->DiagPrinter->errorReport(SourceLoc, "unbound function " + Name);
        exit(1);
      }
    K->DiagPrinter->errorReport(SourceLoc, "unbound symbol " + Name);
    exit(1);
  }
}

void Resolve::resolveOperandsOfUser(User *U, BasicBlock *BB) {
  for (Use &ThisUse : U->uses()) {
    Value *V = ThisUse;
    if (auto R = dyn_cast<UnresolvedValue>(V))
      lookupReplaceUse(R, ThisUse, BB);

    /// If this Operand is a User in itself, resolve recursively
    if (auto W = dyn_cast<User>(V))
      resolveOperandsOfUser(W, BB);
  }
}

void Resolve::runOnFunction(Function *F) {
  for (auto &Arg : F->args())
    K->Map.add(Arg, F->getEntryBlock());

  /// For all statements of the form:
  ///   %V = 7;
  ///      ^
  ///   MallocInst
  ///
  /// Insert into K->Map
  for (auto &BB : *F)
    for (auto It = BB->begin(); It != BB->end(); ++It) {
      auto &V = *It;
      if (auto M = dyn_cast<MallocInst>(V))
        if (!K->Map.add(M, BB)) {
          auto ErrMsg = "symbol " + M->getName() + " already bound";
          K->DiagPrinter->errorReport(M->getSourceLocation(), ErrMsg);
          exit(1);
        }
    }

  /// Resolve operands of all Users:
  ///   %V(...);
  ///    ^  ^---- Operands
  ///   User
  for (auto &BB : *F)
    for (auto &V : *BB)
      resolveOperandsOfUser(cast<User>(V), BB);
}

void Resolve::runOnModule(Module *M) {
  K = M->getContext();
  for (auto P : Externals::get(K)->getProtos())
    if (!K->Map.add(P)) {
      auto ErrMsg = "prototype " + P->getName() + " already bound";
      K->DiagPrinter->errorReport(P->getSourceLocation(), ErrMsg);
      exit(1);
    }
  for (auto &F : *M)
    if (!K->Map.add(F)) {
      auto ErrMsg = "function " + F->getName() + " already exists";
      K->DiagPrinter->errorReport(F->getSourceLocation(), ErrMsg);
      exit(1);
    }
  for (auto &F : *M)
    runOnFunction(F);
}

using KR = Context::ResolutionMap;

bool KR::add(Value *Val, BasicBlock *Block) {
  assert(!isa<UnresolvedValue>(Val) &&
         "Attempting to add an UnresolvedValue to the Map");
  auto &ThisResolutionMap = BlockResolutionMap[Block];
  auto Ret = ThisResolutionMap.insert(std::make_pair(Val->getName(), Val));
  bool NewElementInserted = Ret.second;
  if (!NewElementInserted) {
    auto IteratorToEquivalentKey = Ret.first;
    auto &ValueOfEquivalentKey = IteratorToEquivalentKey->second;
    if (ValueOfEquivalentKey != Val)
      return false;
  }
  return true;
}

bool KR::add(Value *Val, llvm::Value *LLVal) {
  assert(!isa<UnresolvedValue>(Val) &&
         "Attempting to add an UnresolvedValue to the Map");
  auto Ret = LoweringMap.insert(std::make_pair(Val, LLVal));
  bool NewElementInserted = Ret.second;
  if (!NewElementInserted) {
    auto IteratorToEquivalentKey = Ret.first;
    auto &ValueOfEquivalentKey = IteratorToEquivalentKey->second;
    if (ValueOfEquivalentKey != LLVal)
      return false;
  }
  return true;
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
  if (!Block)
    return;
  AllPreds.push_back(Block);
  if (Block->hasNoPredecessors())
    return;
  if (auto OnlyPred = Block->getUniquePredecessor()) {
    flattenPredecessors(OnlyPred, AllPreds);
  } else { // Merge block
    auto FirstPred = *(Block->pred_begin());
    auto PredOfPred = FirstPred->getUniquePredecessor();
    assert(PredOfPred && "Malformed branch: no phi block found");
    flattenPredecessors(PredOfPred, AllPreds);
  }
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
  return IteratorToElement == LoweringMap.end() ? nullptr
                                                : IteratorToElement->second;
}
}
