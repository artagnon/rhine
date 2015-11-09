//-*- C++ -*-

#ifndef RHINE_CONSTANT_H
#define RHINE_CONSTANT_H

#include "llvm/IR/Constants.h"
#include "llvm/ADT/iterator_range.h"

#include <string>
#include <vector>
#include <sstream>

#include "rhine/IR/Type.h"
#include "rhine/IR/User.h"
#include "rhine/IR/UnresolvedValue.h"
#include "rhine/IR/BasicBlock.h"

using namespace std;
using namespace llvm;

namespace rhine {
class Context;
class Module;

class Constant : public User, public FoldingSetNode {
public:
  Constant(Type *Ty, RTValue ID, unsigned NumOps = 0, std::string N = "");
  virtual ~Constant();
  static bool classof(const Value *V);
  virtual llvm::Constant *toLL(llvm::Module *M) = 0;

protected:
  virtual void print(std::ostream &Stream) const = 0;
};

class ConstantInt : public Constant {
  int Val;

public:
  ConstantInt(int Val, unsigned Bitwidth, Context *K);
  virtual ~ConstantInt();
  void *operator new(size_t s);
  static ConstantInt *get(int Val, unsigned Bitwidth, Context *K);
  static bool classof(const Value *V);
  int getVal() const;
  unsigned getBitwidth() const;
  static inline void Profile(FoldingSetNodeID &ID, const Type *Ty,
                             const int &Val);
  void Profile(FoldingSetNodeID &ID) const;
  virtual llvm::Constant *toLL(llvm::Module *M) override;

protected:
  virtual void print(std::ostream &Stream) const override;
};

class ConstantBool : public Constant {
  bool Val;

public:
  ConstantBool(bool Val, Context *K);
  virtual ~ConstantBool() {}
  void *operator new(size_t s);
  static ConstantBool *get(bool Val, Context *K);
  static bool classof(const Value *V);
  float getVal() const;
  static inline void Profile(FoldingSetNodeID &ID, const Type *Ty,
                             const bool &Val);
  void Profile(FoldingSetNodeID &ID) const;
  virtual llvm::Constant *toLL(llvm::Module *M) override;

protected:
  virtual void print(std::ostream &Stream) const override;
};

class ConstantFloat : public Constant {
  float Val;

public:
  ConstantFloat(float Val, Context *K);
  virtual ~ConstantFloat() {}
  void *operator new(size_t s);
  static ConstantFloat *get(float Val, Context *K);
  static bool classof(const Value *V);
  float getVal() const;
  static inline void Profile(FoldingSetNodeID &ID, const Type *Ty,
                             const float &Val);
  void Profile(FoldingSetNodeID &ID) const;
  virtual llvm::Constant *toLL(llvm::Module *M) override;

protected:
  virtual void print(std::ostream &Stream) const override;
};

class Prototype : public Constant {
  Module *ParentModule;
  std::vector<Argument *> ArgumentList;
  Argument *VariadicRestLoadInst;

public:
  Prototype(std::string Name, FunctionType *FTy, RTValue RTTy = RT_Prototype);
  virtual ~Prototype();
  void *operator new(size_t s);
  static Prototype *get(std::string Name, FunctionType *FTy);
  static bool classof(const Value *V);
  void setParent(Module *Parent);
  Module *getParent() const;
  virtual std::string getMangledName() const;
  llvm::Function *getOrInsert(llvm::Module *M);
  void setArguments(std::vector<Argument *> &L);
  void setVariadicRest(Argument *Rest);
  std::vector<Argument *> getArguments() const;
  typedef std::vector<Argument *>::iterator arg_iterator;
  arg_iterator arg_begin();
  arg_iterator arg_end();
  iterator_range<arg_iterator> args();
  virtual llvm::Constant *toLL(llvm::Module *M) override;

protected:
  virtual void print(std::ostream &Stream) const override;
  void emitArguments(std::ostream &Stream) const;
};

class Function : public Prototype {
  std::vector<BasicBlock *> Val;

public:
  Function(std::string Name, FunctionType *FTy);
  virtual ~Function();
  void *operator new(size_t s);
  static Function *get(std::string Name, FunctionType *FTy);
  static bool classof(const Value *V);
  virtual std::string getMangledName() const override;
  void push_back(BasicBlock *NewBB);
  BasicBlock *front() const;
  BasicBlock *back() const;
  BasicBlock *getEntryBlock() const;
  BasicBlock *getExitBlock() const;
  typedef std::vector<BasicBlock *>::iterator iterator;
  iterator begin();
  iterator end();
  virtual llvm::Constant *toLL(llvm::Module *M) override;

protected:
  virtual void print(std::ostream &Stream) const override;
};

class Pointer : public Constant {
  Value *Val;

public:
  Pointer(Value *V, Type *Ty);
  virtual ~Pointer();
  void *operator new(size_t s);
  static Pointer *get(Value *V);
  static bool classof(const Value *V);
  void setVal(Value *V);
  Value *getVal() const;
  static inline void Profile(FoldingSetNodeID &ID, const Type *Ty,
                             const Value *Val);
  void Profile(FoldingSetNodeID &ID) const;
  virtual llvm::Constant *toLL(llvm::Module *M) override;

protected:
  virtual void print(std::ostream &Stream) const override;
};
}

#endif
