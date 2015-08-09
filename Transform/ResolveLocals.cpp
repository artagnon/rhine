#include "rhine/IR.h"
#include "rhine/Transform/ResolveLocals.h"

namespace rhine {
class ResolutionMap {
  typedef std::map <std::string, ValueRef> BlockVR;
  std::map<BasicBlock *, BlockVR> FunctionVR;
public:
  void addMapping(std::string Name, BasicBlock *Block,
                  Value *Val, llvm::Value *LLVal = nullptr) {
    auto ThisVRMap = FunctionVR[Block];
    auto Ret = ThisVRMap.insert(std::make_pair(Name, ValueRef(Val, LLVal)));
    auto NewElementInserted = Ret.second;
    if (!NewElementInserted) {
      auto IteratorToEquivalentKey = Ret.first;
      auto ValueRefOfEquivalentKey = IteratorToEquivalentKey->second;
      if (Val) ValueRefOfEquivalentKey.Val = Val;
      if (LLVal) ValueRefOfEquivalentKey.LLVal = LLVal;
    }
  }
  ValueRef *getMapping(std::string Name, BasicBlock *Block) {
    auto ThisVRMapping = FunctionVR[Block];
    auto IteratorToElement = ThisVRMapping.find(Name);
    return IteratorToElement == ThisVRMapping.end() ? nullptr :
      &IteratorToElement->second;
  }
} Map;

void ResolveLocals::runOnFunction(Function *F) {
  for (auto Arg : F->args()) {
    Map.addMapping(Arg->getName(), F->getEntryBlock(), Arg);
  }
  for (auto V : *F) {
    if (auto M = dyn_cast<MallocInst>(V)) {
      Map.addMapping(M->getName(), F->getEntryBlock(), M);
    }
  }
  for (auto V : *F) {
    auto Name = V->getName();
    if (auto U = dyn_cast<UnresolvedValue>(V)) {
      if (auto S = lookupNameinBlock(Name, F->getEntryBlock())) {
        U->getUse()->set(S);
      } else {
        auto SourceLoc = U->getSourceLocation();
        auto K = F->getContext();
        K->DiagPrinter->errorReport(SourceLoc, "unbound symbol " + Name);
        exit(1);
      }
    }
  }
}

std::vector<BasicBlock *> ResolveLocals::getBlocksInScope(BasicBlock *BB) {
  std::vector<BasicBlock *> Ret;
  Ret.push_back(BB);
  return Ret;
}

Value *ResolveLocals::lookupNameinBlock(std::string Name, BasicBlock *BB) {
  for (auto Block: getBlocksInScope(BB)) {
    if (auto Resolution = Map.getMapping(Name, Block))
      return Resolution->Val;
  }
  return nullptr;
}
}
