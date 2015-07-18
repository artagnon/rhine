//-*- C++ -*-

#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include "llvm/IR/Value.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/ADT/FoldingSet.h"

#include <cstdlib>
#include <map>

#include "location.hh"
#include "rhine/Diagnostic.h"

namespace rhine {
class FunctionType;
class PointerType;
class IntegerType;
class Type;

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
  llvm::LLVMContext &LLContext;
  llvm::IRBuilder<> *Builder;
  DiagnosticPrinter *DiagPrinter;
  struct Externals *ExternalsCache;

  Context(std::ostream &ErrStream = std::cerr):
      LLContext(llvm::getGlobalContext()),
      Builder(new llvm::IRBuilder<>(LLContext)),
      DiagPrinter(new DiagnosticPrinter(&ErrStream)),
      ExternalsCache(nullptr) {}

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
};
}

#endif
