//-*- C++ -*-

#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include <map>
#include "llvm/IR/Value.h"

namespace rhine {
class SymbolTable {
  std::map <class Symbol *, llvm::Value *> Mapping;
public:
  bool addMapping(class Symbol *S, llvm::Value *V) {
    if (Mapping.find(S) != Mapping.end())
      return false;
    Mapping.insert(std::make_pair(S, V));
    return true;
  }
  llvm::Value *getMapping(class Symbol *S) {
    auto V = Mapping.find(S);
    return V == Mapping.end() ? nullptr : V->second;
  }
};
}

#endif
