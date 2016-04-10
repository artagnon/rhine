#include "rhine/Transform/Resolve.hpp"
#include "rhine/Diagnostic/Diagnostic.hpp"
#include "rhine/Externals.hpp"
#include "rhine/IR/BasicBlock.hpp"
#include "rhine/IR/Context.hpp"
#include "rhine/IR/Instruction.hpp"
#include "rhine/IR/Module.hpp"
#include "rhine/IR/UnresolvedValue.hpp"

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
      if (isa<StoreInst>(U->getUser()))
        U.set(M);
      else {
        auto Replacement = LoadInst::get(M);
        Replacement->setSourceLocation(V->getSourceLocation());
        U.set(Replacement);
      }
    } else if (isa<BindInst>(S) || isa<Argument>(S)) {
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
    if (auto Inst = dyn_cast<CallInst>(U->getUser()))
      if (Inst->getCallee() == V) {
        DiagnosticPrinter(SourceLoc) << "unbound function " + Name;
        exit(1);
      }
    DiagnosticPrinter(SourceLoc) << "unbound symbol " + Name;
    exit(1);
  }
}

void Resolve::resolveOperandsOfUser(User *U, BasicBlock *BB) {
  for (Use &ThisUse : U->uses()) {
    Value *V = ThisUse;
    if (auto R = dyn_cast<UnresolvedValue>(V))
      lookupReplaceUse(R, ThisUse, BB);
  }
}

void Resolve::runOnFunction(Function *F) {
  for (auto &Arg : F->args())
    if (!K->Map.add(Arg, F->getEntryBlock())) {
      DiagnosticPrinter(Arg->getSourceLocation())
          << "argument " + Arg->getName() + " attempting to overshadow "
                                            "previously bound symbol with "
                                            "same name";
    }

  // For all statements of the form:
  //   %V = 7;
  //      ^
  //   BindInst
  //
  // Insert into K->Map
  for (auto &BB : *F)
    for (auto &V : *BB) {
      if (auto B = dyn_cast<BindInst>(V))
        if (!K->Map.add(B, BB)) {
          DiagnosticPrinter(B->getSourceLocation())
              << "symbol " + B->getName() + " attempting to overshadow "
                                            "previously bound symbol with same "
                                            "name";
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
      auto ErrMsg =
          "prototype " + P->getName() +
          " attempting to overshadow previously bound symbol with same name";
      DiagnosticPrinter(P->getSourceLocation()) << ErrMsg;
      exit(1);
    }
  for (auto &F : *M)
    if (!K->Map.add(F)) {
      DiagnosticPrinter(F->getSourceLocation())
          << "function " + F->getName() + " attempting to overshadow "
                                          "previously bound symbol with same "
                                          "name";
      exit(1);
    }
  for (auto &F : *M)
    runOnFunction(F);
}

using KR = Context::ResolutionMap;

bool KR::add(Value *Val, BasicBlock *Block) {
  assert(!isa<UnresolvedValue>(Val) &&
         "Attempting to add an UnresolvedValue to the Map");
  if (get(Val, Block))
    return false;
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

Value *KR::searchOneBlock(std::string Name, BasicBlock *Block) {
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

Value *KR::get(std::string Name, BasicBlock *Block) {
  std::list<BasicBlock *> UniqPreds;
  flattenPredecessors(Block, UniqPreds);
  UniqPreds.unique();
  UniqPreds.push_back(nullptr); // Global
  for (auto BB : UniqPreds)
    if (auto Result = searchOneBlock(Name, BB))
      return Result;
  return nullptr;
}

Value *KR::get(Value *Val, BasicBlock *Block) {
  return get(Val->getName(), Block);
}

llvm::Value *KR::getl(Value *Val) {
  auto IteratorToElement = LoweringMap.find(Val);
  return IteratorToElement == LoweringMap.end() ? nullptr
                                                : IteratorToElement->second;
}
}
