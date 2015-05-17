//-*- C++ -*-

#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include "llvm/IR/DerivedTypes.h"

#include <string>
#include <vector>
#include <sstream>

#include "rhine/Context.h"
#include "rhine/IR/Value.h"

namespace rhine {
class Instruction : public Value {
protected:
  std::vector<Value *> OperandList;
public:
  Instruction(Type *Ty, RTValue ID) : Value(Ty, ID) {}
  void addOperand(Value *V) {
    OperandList.push_back(V);
  }
  Value *getOperand(unsigned i) {
    return OperandList[i];
  }
  friend ostream &operator<<(ostream &Stream, const Instruction &I) {
    I.print(Stream);
    return Stream;
  }
  virtual Type *typeInfer(Context *K = nullptr) = 0;
  virtual llvm::Value *toLL(llvm::Module *M = nullptr, Context *K = nullptr) = 0;
protected:
  virtual void print(std::ostream &Stream) const = 0;
};

class AddInst : public Instruction {
public:
  AddInst(Type *Ty) : Instruction(Ty, RT_AddInst) {}
  static AddInst *get(Type *Ty, Context *K) {
    return new (K->RhAllocator) AddInst(Ty);
  }
  static bool classof(const Value *V) {
    return V->getValID() == RT_AddInst;
  }
  friend ostream &operator<<(ostream &Stream, const AddInst &A) {
    A.print(Stream);
    return Stream;
  }
  Type *typeInfer(Context *K = nullptr);
  llvm::Value *toLL(llvm::Module *M = nullptr, Context *K = nullptr);
protected:
  virtual void print(std::ostream &Stream) const {
    Stream << "+ ~" << *getType() << std::endl;
    for (auto O: OperandList)
      Stream << *O << std::endl;
  }
};

class CallInst : public Instruction {
  std::string Name;
public:
  // May be untyped
  CallInst(std::string FunctionName, Type *Ty) :
      Instruction(Ty, RT_CallInst), Name(FunctionName) {}
  static CallInst *get(std::string FunctionName, Type *Ty, Context *K) {
    return new (K->RhAllocator) CallInst(FunctionName, Ty);
  }
  static bool classof(const Value *V) {
    return V->getValID() == RT_CallInst;
  }
  std::string getName() {
    return Name;
  }
  friend ostream &operator<<(ostream &Stream, const CallInst &C) {
    C.print(Stream);
    return Stream;
  }
  Type *typeInfer(Context *K = nullptr);
  llvm::Value *toLL(llvm::Module *M = nullptr, Context *K = nullptr);
protected:
  virtual void print(std::ostream &Stream) const {
    Stream << Name << " ~" << *getType() << std::endl;
    for (auto O: OperandList)
      Stream << *O << std::endl;
  }
};

class BindInst : public Instruction {
  std::string Name;
  Value *Val;
public:
  // This instruction cannot be an rvalue, and is of type Void
  BindInst(std::string N, Value *V) : Instruction(nullptr, RT_BindInst),
                                      Name(N), Val(V) {}

  static BindInst *get(std::string N, Value *V, Context *K) {
    return new (K->RhAllocator) BindInst(N, V);
  }
  static bool classof(const Value *V) {
    return V->getValID() == RT_BindInst;
  }
  Value *getVal() {
    return Val;
  }
  std::string getName() {
    return Name;
  }
  friend ostream &operator<<(ostream &Stream, const BindInst &S) {
    S.print(Stream);
    return Stream;
  }
  // Infers to Void, lowers to nothing
  Type *typeInfer(Context *K = nullptr);
  llvm::Value *toLL(llvm::Module *M = nullptr, Context *K = nullptr);
protected:
  virtual void print(std::ostream &Stream) const {
    Stream << Name << " = " << *Val;
  }
};
}

#endif
