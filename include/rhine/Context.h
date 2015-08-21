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
  llvm::LLVMContext &LLContext;
  llvm::IRBuilder<> *Builder;
  DiagnosticPrinter *DiagPrinter;
  Externals *ExternalsCache;
  llvm::Function *CurrentFunction;

  Context(std::ostream &ErrStream = std::cerr):
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

  struct ValueRef {
    Value *Val;
    llvm::Value *LLVal;
    ValueRef(Value *Val, llvm::Value *LLVal) : Val(Val), LLVal(LLVal) {}
  };

  class ResolutionMap {
    typedef std::map <std::string, ValueRef> BlockVR;
    std::map<BasicBlock *, BlockVR> FunctionVR;
    Value *searchOneBlock(Value *Val, BasicBlock *Block);
  public:
    void add(Value *Val, BasicBlock *Block = nullptr,
             llvm::Value *LLVal = nullptr);
    Value *get(Value *Val, BasicBlock *Block = nullptr);
    llvm::Value *getl(Value *Val, BasicBlock *Block = nullptr);
  };

  ResolutionMap Map;
};
}

#endif
