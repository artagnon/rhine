//-*- C++ -*-

#ifndef RHINE_CONSTANT_H
#define RHINE_CONSTANT_H

#include "llvm/IR/Constants.h"
#include "llvm/ADT/iterator_range.h"

#include <string>
#include <vector>
#include <sstream>

#include "rhine/Context.h"
#include "rhine/IR/User.h"
#include "rhine/IR/UnresolvedValue.h"
#include "rhine/IR/BasicBlock.h"

using namespace std;
using namespace llvm;

namespace rhine {
class Module;

class Constant : public User {
public:
  Constant(Type *Ty, RTValue ID);
  virtual ~Constant() {}
  static bool classof(const Value *V);
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
  llvm::Constant *toLL(llvm::Module *M, Context *K);
protected:
  virtual void print(std::ostream &Stream) const;
};

class ConstantBool : public Constant {
  bool Val;
public:
  ConstantBool(bool Val, Context *K);
  virtual ~ConstantBool() {}
  static ConstantBool *get(bool Val, Context *K);
  static bool classof(const Value *V);
  float getVal();
  llvm::Constant *toLL(llvm::Module *M, Context *K);
protected:
  virtual void print(std::ostream &Stream) const;
};

class ConstantFloat : public Constant {
public:
  float Val;
  ConstantFloat(float Val, Context *K);
  virtual ~ConstantFloat() {}
  static ConstantFloat *get(float Val, Context *K);
  static bool classof(const Value *V);
  float getVal();
  llvm::Constant *toLL(llvm::Module *M, Context *K);
protected:
  virtual void print(std::ostream &Stream) const;
};

class Function : public User {
  Module *ParentModule;
  std::string Name;
  std::vector<Argument *> ArgumentList;
  Argument *VariadicRestLoadInst;
  BasicBlock *Val;
public:
  Function(FunctionType *FTy);
  virtual ~Function() {}
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
  BasicBlock *getEntryBlock();
  BasicBlock::iterator begin();
  BasicBlock::iterator end();
  typedef std::vector<Argument *>::iterator arg_iterator;
  arg_iterator arg_begin();
  arg_iterator arg_end();
  iterator_range<arg_iterator> args();
  llvm::Constant *toLL(llvm::Module *M, Context *K);
protected:
  virtual void print(std::ostream &Stream) const;
private:
  void emitArguments(std::ostream &Stream) const;
};
}

#endif
