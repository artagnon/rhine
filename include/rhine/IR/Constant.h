//-*- C++ -*-

#ifndef RHINE_CONSTANT_H
#define RHINE_CONSTANT_H

#include "llvm/IR/Constants.h"
#include "llvm/ADT/iterator_range.h"

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
  static ConstantInt *get(int Val, unsigned Bitwidth, Context *K);
  static bool classof(const Value *V);
  int getVal();
  unsigned getBitwidth();
  friend ostream &operator<<(ostream &Stream, const ConstantInt &I) {
    I.print(Stream);
    return Stream;
  }
  llvm::Constant *toLL(llvm::Module *M, Context *K);
protected:
  virtual void print(std::ostream &Stream) const;
};

class ConstantBool : public Constant {
  bool Val;
public:
  ConstantBool(bool Val, Context *K);
  static ConstantBool *get(bool Val, Context *K);
  static bool classof(const Value *V);
  float getVal();
  friend ostream &operator<<(ostream &Stream, const ConstantBool &B) {
    B.print(Stream);
    return Stream;
  }
  llvm::Constant *toLL(llvm::Module *M, Context *K);
protected:
  virtual void print(std::ostream &Stream) const;
};

class ConstantFloat : public Constant {
public:
  float Val;
  ConstantFloat(float Val, Context *K);
  static ConstantFloat *get(float Val, Context *K);
  static bool classof(const Value *V);
  float getVal();
  friend ostream &operator<<(ostream &Stream, const ConstantFloat &F) {
    F.print(Stream);
    return Stream;
  }
  llvm::Constant *toLL(llvm::Module *M, Context *K);
protected:
  virtual void print(std::ostream &Stream) const;
};

class Function : public Value {
  Module *ParentModule;
  std::string Name;
  std::vector<Argument *> ArgumentList;
  Argument *VariadicRestLoadInst;
  BasicBlock *Val;
public:
  Function(FunctionType *FTy);
  static Function *get(FunctionType *FTy, Context *K);
  static bool classof(const Value *V);
  void setParent(Module *Parent);
  Module *getParent();
  void setName(std::string N);
  std::string getName();
  void setArguments(std::vector<Argument *> L);
  void setVariadicRest(Argument *Rest);
  std::vector<Argument *> getArguments();
  void setBody(BasicBlock *Body);
  BasicBlock *getVal();
  BasicBlock::iterator begin();
  BasicBlock::iterator end();
  typedef std::vector<Argument *>::iterator arg_iterator;
  arg_iterator arg_begin();
  arg_iterator arg_end();
  iterator_range<arg_iterator> args();
  friend ostream &operator<<(ostream &Stream, const Function &F) {
    F.print(Stream);
    return Stream;
  }
  llvm::Constant *toLL(llvm::Module *M, Context *K);
protected:
  virtual void print(std::ostream &Stream) const;
private:
  void emitArguments(std::ostream &Stream) const;
};
}

#endif
