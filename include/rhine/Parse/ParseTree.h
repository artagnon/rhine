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
  PTree(Context *K) : M(Module::get(K)), VirtualRegisterNumber(0) {}
  Module *M;
  std::string getVirtualRegisterName() {
    return "rhv" + std::to_string(VirtualRegisterNumber++);
  }
private:
  unsigned VirtualRegisterNumber;
};
}

#endif
