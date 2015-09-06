//-*- C++ -*-

#ifndef PARSETREE_H
#define PARSETREE_H

#include "rhine/IR/Module.h"
#include <vector>

using namespace std;

namespace rhine {
class Argument;
class Type;
class Function;
class Context;

class ArgumentList : public std::vector<Argument *> {
  bool VariadicFlag = false;
public:
  bool isVariadic() {
    return VariadicFlag;
  }
  void setIsVariadic(bool toSet) {
    VariadicFlag = toSet;
  }
};

class TypeList : public std::vector<Type *> {
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
  Module *M;
  PTree(Context *K) : M(Module::get(K)), VirtualRegisterNumber(0) {}
  std::string getVirtualRegisterName() {
    return "rhv" + std::to_string(VirtualRegisterNumber++);
  }
  void appendFunction(Function *F) {
    M->appendFunction(F);
  }
private:
  unsigned VirtualRegisterNumber;
};
}

#endif
