//-*- C++ -*-

#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include "llvm/IR/DerivedTypes.h"

#include <string>
#include <vector>
#include <sstream>

#include "rhine/Context.h"
#include "rhine/IR/Type.h"
#include "rhine/IR/Value.h"
#include "rhine/IR/BasicBlock.h"

namespace rhine {
class Instruction : public Value {
protected:
  std::vector<Value *> OperandList;
public:
  Instruction(Type *Ty, RTValue ID, std::string Name = "");
  void addOperand(Value *V);
  Value *getOperand(unsigned i);
  std::vector<Value *> getOperands();
  void setOperands(std::vector<Value *> Ops);
  virtual llvm::Value *toLL(llvm::Module *M, Context *K) = 0;
protected:
  virtual void print(std::ostream &Stream) const = 0;
};

class AddInst : public Instruction {
public:
  AddInst(Type *Ty);
  static AddInst *get(Context *K);
  static bool classof(const Value *V);
  llvm::Value *toLL(llvm::Module *M, Context *K) override;
protected:
  void print(std::ostream &Stream) const override;
};

class CallInst : public Instruction {
  std::string Callee;
public:
  CallInst(std::string FunctionName, Type *Ty);
  static CallInst *get(std::string FunctionName, Context *K);
  static bool classof(const Value *V);
  std::string getCallee();
  llvm::Value *toLL(llvm::Module *M, Context *K) override;
protected:
  void print(std::ostream &Stream) const override;
};

class MallocInst : public Instruction {
  Value *Val;
public:
  MallocInst(std::string N, Type *Ty, Value *V);
  static MallocInst *get(std::string N, Value *V, Context *K);
  static bool classof(const Value *V);
  void setVal(Value *V);
  Value *getVal();
  llvm::Value *toLL(llvm::Module *M, Context *K) override;
protected:
  void print(std::ostream &Stream) const override;
};

class LoadInst : public Instruction {
public:
  LoadInst(std::string N, Type *T, RTValue ID = RT_LoadInst);
  static LoadInst *get(std::string N, Type *T, Context *K);
  static bool classof(const Value *V);
  llvm::Value *toLL(llvm::Module *M, Context *K) override;
protected:
  void print(std::ostream &Stream) const override;
};

class IfInst : public Instruction {
  Value *Conditional;
  BasicBlock *TrueBB;
  BasicBlock *FalseBB;
public:
  IfInst(Type *Ty, Value * Conditional_,
         BasicBlock *TrueBB_, BasicBlock *FalseBB_);
  static IfInst *get(Value * Conditional, BasicBlock *TrueBB,
                     BasicBlock *FalseBB, Context *K);
  static bool classof(const Value *V);
  Value *getConditional();
  void setConditional(Value *C);
  BasicBlock *getTrueBB();
  BasicBlock *getFalseBB();
  llvm::Value *toLL(llvm::Module *M, Context *K) override;
protected:
  void print(std::ostream &Stream) const override;
};
}

#endif
