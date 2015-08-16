//-*- C++ -*-

#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/ADT/FoldingSet.h"

#include <cstdlib>
#include <map>

#include "location.hh"
#include "rhine/IR/Type.h"
#include "rhine/IR/BasicBlock.h"
#include "rhine/Diagnostic.h"

namespace rhine {
class Value;
class Externals;

struct SymbolRef {
  class Type *Ty;
  llvm::Value *LLVal;
  SymbolRef(Type *Typ, llvm::Value *Val) : Ty(Typ), LLVal(Val) {}
};

struct ValueRef {
  Value *Val;
  llvm::Value *LLVal;
  ValueRef(Value *Val, llvm::Value *LLVal) : Val(Val), LLVal(LLVal) {}
};

class Context {
  std::map <std::string, SymbolRef> SymbolMapping;

public:
  llvm::BumpPtrAllocator RhAllocator;
  llvm::FoldingSet<FunctionType> FTyCache;
  llvm::FoldingSet<PointerType> PTyCache;
  llvm::FoldingSet<IntegerType> ITyCache;
  UnType UniqueUnType;
  VoidType UniqueVoidType;
  BoolType UniqueBoolType;
  FloatType UniqueFloatType;
  StringType UniqueStringType;
  BasicBlock *GlobalBBHandle;
  llvm::LLVMContext &LLContext;
  llvm::IRBuilder<> *Builder;
  DiagnosticPrinter *DiagPrinter;
  Externals *ExternalsCache;
  llvm::Function *CurrentFunction;

  Context(std::ostream &ErrStream = std::cerr):
      GlobalBBHandle(nullptr),
      LLContext(llvm::getGlobalContext()),
      Builder(new llvm::IRBuilder<>(LLContext)),
      DiagPrinter(new DiagnosticPrinter(&ErrStream)),
      ExternalsCache(nullptr),
      CurrentFunction(nullptr) {}

  // The big free
  void releaseMemory() {
    RhAllocator.Reset();
    FTyCache.clear();
    ITyCache.clear();
    SymbolMapping.clear();
  }

  void addMapping(std::string S, Type *Ty, llvm::Value *V = nullptr) {
    auto Ret = SymbolMapping.insert(std::make_pair(S, SymbolRef(Ty, V)));
    if (!Ret.second) {
      if (Ty) Ret.first->second.Ty = Ty;
      if (V) Ret.first->second.LLVal = V;
    }
  }
  Type *getMappingTy(std::string S) {
    auto R = SymbolMapping.find(S);
    return R == SymbolMapping.end() ? nullptr : R->second.Ty;
  }
  llvm::Value *getMappingVal(std::string S) {
    auto R = SymbolMapping.find(S);
    return R == SymbolMapping.end() ? nullptr : R->second.LLVal;
  }
  llvm::Value *getMappingVal(std::string S, location SourceLoc) {
    auto R = SymbolMapping.find(S);
    if (R == SymbolMapping.end() || !R->second.LLVal) {
      DiagPrinter->errorReport(SourceLoc, "unbound symbol " + S);
      exit(1);
    }
    return R->second.LLVal;
  }

  class ResolutionMap {
    typedef std::map <std::string, ValueRef> BlockVR;
    std::map<BasicBlock *, BlockVR> FunctionVR;
  public:
    void addMapping(std::string Name, BasicBlock *Block,
                    Value *Val, llvm::Value *LLVal = nullptr) {
      auto &ThisVRMap = FunctionVR[Block];
      auto Ret = ThisVRMap.insert(std::make_pair(Name, ValueRef(Val, LLVal)));
      auto &NewElementInserted = Ret.second;
      if (!NewElementInserted) {
        auto IteratorToEquivalentKey = Ret.first;
        auto ValueRefOfEquivalentKey = IteratorToEquivalentKey->second;
        if (Val) ValueRefOfEquivalentKey.Val = Val;
        if (LLVal) ValueRefOfEquivalentKey.LLVal = LLVal;
      }
    }
    ValueRef *getMapping(std::string Name, BasicBlock *Block) {
      auto &ThisVRMap = FunctionVR[Block];
      auto IteratorToElement = ThisVRMap.find(Name);
      return IteratorToElement == ThisVRMap.end() ? nullptr :
        &IteratorToElement->second;
    }
  };
  ResolutionMap Map;
};
}

#endif
