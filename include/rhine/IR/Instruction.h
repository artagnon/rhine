//-*- C++ -*-

#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include "llvm/IR/Value.h"

#include <string>
#include <vector>
#include <sstream>

#include "rhine/Context.h"
#include "rhine/IR/Type.h"
#include "rhine/IR/Value.h"
#include "rhine/IR/BasicBlock.h"
#include "rhine/IR/User.h"

namespace rhine {
class Use;

class Instruction : public User {
public:
  Instruction(Type *Ty, RTValue ID, unsigned NumOps, std::string Name = "");
  virtual ~Instruction() {}
  static bool classof(const Value *V);
  virtual llvm::Value *toLL(llvm::Module *M) = 0;
protected:
  virtual void print(std::ostream &Stream) const = 0;
};

class AddInst : public Instruction {
public:
  AddInst(Type *Ty);
  virtual ~AddInst() {}
  void *operator new(size_t s);
  static AddInst *get(Context *K);
  static bool classof(const Value *V);
  virtual llvm::Value *toLL(llvm::Module *M) override;
protected:
  void print(std::ostream &Stream) const override;
};

class CallInst : public Instruction {
public:
  CallInst(Type *Ty, unsigned NumOps);
  virtual ~CallInst() {}
  void *operator new(size_t s, unsigned n);
  static CallInst *get(Value *Callee, std::vector<Value *> Ops);
  static bool classof(const Value *V);
  Value *getCallee() const;
  virtual llvm::Value *toLL(llvm::Module *M) override;
protected:
  void print(std::ostream &Stream) const override;
};

class MallocInst : public Instruction {
public:
  MallocInst(std::string N, Type *Ty);
  virtual ~MallocInst() {}
  void *operator new(size_t s);
  static MallocInst *get(std::string N, Value *V, Context *K);
  static bool classof(const Value *V);
  void setVal(Value *V);
  Value *getVal();
  virtual llvm::Value *toLL(llvm::Module *M) override;
protected:
  void print(std::ostream &Stream) const override;
};

class LoadInst : public Instruction {
public:
  LoadInst(Value *V, std::string Name);
  virtual ~LoadInst() {}
  void *operator new(size_t s);
  static LoadInst *get(Value *V, std::string Name);
  Value *getVal() const;
  static bool classof(const Value *V);
  virtual llvm::Value *toLL(llvm::Module *M) override;
protected:
  void print(std::ostream &Stream) const override;
};

class ReturnInst : public Instruction {
public:
  ReturnInst(Type *Ty, bool IsNotVoid);
  virtual ~ReturnInst() {}
  void *operator new(size_t s, unsigned NumberOfArgs);
  static ReturnInst *get(Value *V, Context *K);
  static bool classof(const Value *V);
  void setVal(Value *V);
  Value *getVal();
  virtual llvm::Value *toLL(llvm::Module *M) override;
protected:
  void print(std::ostream &Stream) const override;
};

class IfInst : public Instruction {
public:
  IfInst(Type *Ty);
  virtual ~IfInst() {}
  void *operator new(size_t s);
  static IfInst *get(Value * Conditional, BasicBlock *TrueBB,
                     BasicBlock *FalseBB, Context *K);
  static bool classof(const Value *V);
  Value *getConditional() const;
  void setConditional(Value *C);
  BasicBlock *getTrueBB() const;
  BasicBlock *getFalseBB() const;
  virtual llvm::Value *toLL(llvm::Module *M) override;
protected:
  void print(std::ostream &Stream) const override;
};
}

#endif
