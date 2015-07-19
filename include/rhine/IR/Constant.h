//-*- C++ -*-

#ifndef RHINE_CONSTANT_H
#define RHINE_CONSTANT_H

#include "llvm/IR/Constants.h"

#include <string>
#include <vector>
#include <sstream>

#include "rhine/Context.h"
#include "rhine/IR/Value.h"
#include "rhine/IR/BasicBlock.h"

using namespace std;
using namespace llvm;

namespace rhine {
class Module;

class Constant : public Value {
public:
  Constant(Type *Ty, RTValue ID);
  friend ostream &operator<<(ostream &Stream, const Constant &C) {
    C.print(Stream);
    return Stream;
  }
  llvm::Constant *toLL(llvm::Module *M, Context *K) = 0;
protected:
  virtual void print(std::ostream &Stream) const = 0;
};

class ConstantInt : public Constant {
  int Val;
public:
  ConstantInt(int Val, unsigned Bitwidth, Context *K);
  static ConstantInt *get(int Val, unsigned Bitwidth, Context *K) {
    return new (K->RhAllocator) ConstantInt(Val, Bitwidth, K);
  }
  static bool classof(const Value *V) {
    return V->getValID() == RT_ConstantInt;
  }
  int getVal() {
    return Val;
  }
  unsigned getBitwidth() {
    if (auto ITy = dyn_cast<IntegerType>(VTy))
      return ITy->getBitwidth();
    assert(0 && "ConstantInt of non IntegerType type");
  }

  friend ostream &operator<<(ostream &Stream, const ConstantInt &I) {
    I.print(Stream);
    return Stream;
  }
  llvm::Constant *toLL(llvm::Module *M, Context *K);
protected:
  virtual void print(std::ostream &Stream) const {
    Stream << Val << " ~" << *getType();
  }
};

class ConstantBool : public Constant {
  bool Val;
public:
  ConstantBool(bool Val, Context *K) :
      Constant(BoolType::get(K), RT_ConstantBool), Val(Val) {}
  static ConstantBool *get(bool Val, Context *K) {
    return new (K->RhAllocator) ConstantBool(Val, K);
  }
  static bool classof(const Value *V) {
    return V->getValID() == RT_ConstantBool;
  }
  float getVal() {
    return Val;
  }
  friend ostream &operator<<(ostream &Stream, const ConstantBool &B) {
    B.print(Stream);
    return Stream;
  }
  llvm::Constant *toLL(llvm::Module *M, Context *K);
protected:
  virtual void print(std::ostream &Stream) const {
    Stream << Val << " ~" << *getType();
  }
};

class ConstantFloat : public Constant {
public:
  float Val;
  ConstantFloat(float Val, Context *K) :
      Constant(FloatType::get(K), RT_ConstantFloat), Val(Val) {}
  static ConstantFloat *get(float Val, Context *K) {
    return new (K->RhAllocator) ConstantFloat(Val, K);
  }
  static bool classof(const Value *V) {
    return V->getValID() == RT_ConstantFloat;
  }
  float getVal() {
    return Val;
  }
  friend ostream &operator<<(ostream &Stream, const ConstantFloat &F) {
    F.print(Stream);
    return Stream;
  }
  llvm::Constant *toLL(llvm::Module *M, Context *K);
protected:
  virtual void print(std::ostream &Stream) const {
    Stream << Val << " ~" << *getType();
  }
};

class Function : public Value {
  Module *ParentModule;
  std::string Name;
  std::vector<Symbol *> ArgumentList;
  Symbol *VariadicRestSymbol;
  BasicBlock *Val;
public:
  Function(FunctionType *FTy) :
      Value(FTy, RT_Function), ParentModule(nullptr),
      VariadicRestSymbol(nullptr), Val(nullptr) {}
  static Function *get(FunctionType *FTy, Context *K) {
    return new (K->RhAllocator) Function(FTy);
  }
  static bool classof(const Value *V) {
    return V->getValID() == RT_Function;
  }
  void setParent(Module *Parent) {
    ParentModule = Parent;
  }
  Module *getParent() {
    return ParentModule;
  }
  void setName(std::string N) {
    Name = N;
  }
  std::string getName() {
    return Name;
  }
  void setArguments(std::vector<Symbol *> L) {
    ArgumentList = L;
  }
  void setVariadicRest(Symbol *Rest) {
    if (!Rest)
      return;
    assert(cast<FunctionType>(VTy)->isVariadic() &&
           "Confusion about whether function is variadic");
    VariadicRestSymbol = Rest;
  }
  std::vector<Symbol *> getArguments() {
    return ArgumentList;
  }
  void setBody(BasicBlock *Body) {
    Val = Body;
  }
  BasicBlock *getVal() {
    return Val;
  }
  friend ostream &operator<<(ostream &Stream, const Function &F) {
    F.print(Stream);
    return Stream;
  }
  llvm::Constant *toLL(llvm::Module *M, Context *K);
  typedef std::vector<Value *>::iterator iterator;
  iterator begin() {
    return Val->ValueList.begin();
  }
  iterator end() {
    return Val->ValueList.end();
  }
protected:
  virtual void print(std::ostream &Stream) const {
    Stream << Name << " ~" << *getType() << std::endl;
    for (auto A: ArgumentList)
      Stream << *A << std::endl;
    if (VariadicRestSymbol)
      Stream << "&" << *VariadicRestSymbol << std::endl;
    for (auto V: Val->ValueList)
      Stream << *V << std::endl;
  }
};
}

#endif
