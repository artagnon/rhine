//-*- C++ -*-

#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include "llvm/IR/Value.h"
#include "llvm/ADT/FoldingSet.h"

#include <map>

namespace rhine {
class Context {
  std::map <std::string, llvm::Value *> SymbolMapping;
  llvm::FoldingSet<class Symbol> SymbolCache;
public:
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
  llvm::FoldingSet<class Symbol> *getSymbolCache() {
    return &SymbolCache;
  }
};
}

#endif
