//-*- C++ -*-

#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include "llvm/IR/Value.h"
#include "llvm/ADT/FoldingSet.h"

#include <map>

namespace rhine {
class Context {
  // For function overloading, NameMapping should map std::string to
  // std::vector<Symbol *>
  std::map <std::string, class Symbol *> NameMapping;
  std::map <std::string, llvm::Value *> SymbolMapping;
public:
  llvm::BumpPtrAllocator RhAllocator;
  llvm::FoldingSet<class Symbol> SymbolCache;
  llvm::FoldingSet<class FunctionType> FTyCache;

  // The big free
  void releaseMemory() {
    RhAllocator.Reset();
    SymbolCache.clear();
    FTyCache.clear();
    NameMapping.clear();
    SymbolMapping.clear();
  }

  //===--------------------------------------------------------------------===//
  // Functions that oeprate on SymbolMapping.
  //===--------------------------------------------------------------------===//
  bool addMapping(std::string S, llvm::Value *V) {
    if (SymbolMapping.find(S) != SymbolMapping.end())
      return false;
    SymbolMapping.insert(std::make_pair(S, V));
    return true;
  }
  llvm::Value *getMapping(std::string S) {
    auto V = SymbolMapping.find(S);
    return V == SymbolMapping.end() ? nullptr : V->second;
  }
  llvm::Value *getMappingOrDie(std::string S) {
    auto V = SymbolMapping.find(S);
    assert(V != SymbolMapping.end() && "Unbound symbol");
    return V->second;
  }
  //===--------------------------------------------------------------------===//
  // Functions that oeprate on NameMapping.
  //===--------------------------------------------------------------------===//
  bool addNameMapping(std::string N, class Symbol *S) {
    if (NameMapping.find(N) != NameMapping.end())
      return false;
    NameMapping.insert(std::make_pair(N, S));
    return true;
  }
  Symbol *getNameMapping(std::string S) {
    auto V = NameMapping.find(S);
    assert(V != NameMapping.end() &&
           "internal error: NameMapping not pre-populated");
    return V->second;
  }
};
}

#endif
