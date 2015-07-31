//-*- C++ -*-

#ifndef PARSETREE_H
#define PARSETREE_H

#include "rhine/IR.h"
#include <vector>

using namespace std;

namespace rhine {
class ArgumentList : public std::vector<class Argument *> {
  bool VariadicFlag = false;
public:
  bool isVariadic() {
    return VariadicFlag;
  }
  void setIsVariadic(bool toSet) {
    VariadicFlag = toSet;
  }
};

class TypeList : public std::vector<class Type *> {
  bool VariadicFlag = false;
public:
  bool isVariadic() {
    return VariadicFlag;
  }
  void setIsVariadic(bool toSet) {
    VariadicFlag = toSet;
  }
};

class PTree {
public:
  Module M;
  PTree() {}
};
}

#endif
