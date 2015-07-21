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
  Instruction(Type *Ty, RTValue ID);
  void addOperand(Value *V);
  Value *getOperand(unsigned i);
  std::vector<Value *> getOperands();
  void setOperands(std::vector<Value *> Ops);
  friend ostream &operator<<(ostream &Stream, const Instruction &I) {
    I.print(Stream);
    return Stream;
  }
  virtual llvm::Value *toLL(llvm::Module *M, Context *K) = 0;
protected:
  virtual void print(std::ostream &Stream) const = 0;
};

class AddInst : public Instruction {
public:
  AddInst(Type *Ty);
  static AddInst *get(Context *K);
  static bool classof(const Value *V);
  friend ostream &operator<<(ostream &Stream, const AddInst &A) {
    A.print(Stream);
    return Stream;
  }
  llvm::Value *toLL(llvm::Module *M, Context *K);
protected:
  virtual void print(std::ostream &Stream) const;
};

class CallInst : public Instruction {
  std::string Name;
public:
  CallInst(std::string FunctionName, Type *Ty);
  static CallInst *get(std::string FunctionName, Context *K);
  static bool classof(const Value *V);
  std::string getName();
  friend ostream &operator<<(ostream &Stream, const CallInst &C) {
    C.print(Stream);
    return Stream;
  }
  llvm::Value *toLL(llvm::Module *M, Context *K);
protected:
  virtual void print(std::ostream &Stream) const;
};

class BindInst : public Instruction {
  std::string Name;
  Value *Val;
public:
  BindInst(std::string N, Type *Ty, Value *V);
  static BindInst *get(std::string N, Value *V, Context *K);
  static bool classof(const Value *V);
  void setVal(Value *V);
  Value *getVal();
  std::string getName();
  friend ostream &operator<<(ostream &Stream, const BindInst &S) {
    S.print(Stream);
    return Stream;
  }
  llvm::Value *toLL(llvm::Module *M, Context *K);
protected:
  virtual void print(std::ostream &Stream) const;
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
  friend ostream &operator<<(ostream &Stream, const IfInst &S) {
    S.print(Stream);
    return Stream;
  }
  llvm::Value *toLL(llvm::Module *M, Context *K);
protected:
  virtual void print(std::ostream &Stream) const;
};
}

#endif
