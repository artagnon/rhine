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
  BasicBlock *Parent;
public:
  /// Number of operands required to initalize properly; NumAllocatedOps and
  /// NumOperands are initialized to this value
  Instruction(Type *Ty, RTValue ID, unsigned NumOps, std::string Name = "");

  virtual ~Instruction() {}
  static bool classof(const Value *V);

  /// Back pointer to parent isn't present in Value, just Instruction
  BasicBlock *getParent() const;
  void setParent(BasicBlock *P);

  virtual llvm::Value *toLL(llvm::Module *M) = 0;
protected:
  virtual void print(std::ostream &Stream) const = 0;
};

class AddInst : public Instruction {
public:
  /// We can explicitly request a particular type from two possibly-different
  /// types being added
  AddInst(Type *Ty, Value *Op0, Value *Op1);

  virtual ~AddInst() {}

  /// Allocate a constant two operands
  void *operator new(size_t S);

  /// Context inferred from Op0
  static AddInst *get(Value *Op0, Value *Op1);
  static bool classof(const Value *V);
  virtual llvm::Value *toLL(llvm::Module *M) override;
protected:
  void print(std::ostream &Stream) const override;
};

class CallInst : public Instruction {
public:
  CallInst(Type *Ty, unsigned NumOps, std::string N);
  virtual ~CallInst() {}
  void *operator new(size_t S, unsigned n);
  static CallInst *get(Value *Callee, std::vector<Value *> Ops);
  static bool classof(const Value *V);
  Value *getCallee() const;
  virtual llvm::Value *toLL(llvm::Module *M) override;
protected:
  void print(std::ostream &Stream) const override;
};

class MallocInst : public Instruction {
public:
  MallocInst(std::string N, Value *V);
  virtual ~MallocInst() {}
  void *operator new(size_t S);
  static MallocInst *get(std::string N, Value *V);
  static bool classof(const Value *V);

  /// Operand0 manipulators
  Value *getVal();
  void setVal(Value *V);

  virtual llvm::Value *toLL(llvm::Module *M) override;
protected:
  void print(std::ostream &Stream) const override;
};

class LoadInst : public Instruction {
public:
  /// Load an existing MallocInst
  LoadInst(MallocInst *M);

  virtual ~LoadInst() {}
  void *operator new(size_t S);
  static LoadInst *get(MallocInst *M);
  Value *getVal() const;
  static bool classof(const Value *V);
  virtual llvm::Value *toLL(llvm::Module *M) override;
protected:
  void print(std::ostream &Stream) const override;
};

class StoreInst : public Instruction {
public:
  /// A socket and plug; they better fit well
  StoreInst(Value *MallocedValue, Value *NewValue);

  virtual ~StoreInst() {}

  /// Allocate a constant two operands
  void *operator new(size_t S);

  static StoreInst *get(Value *MallocedValue, Value *NewValue);

  /// Get operands 0 and 1 respectively
  Value *getMallocedValue() const;
  Value *getNewValue() const;

  static bool classof(const Value *V);
  virtual llvm::Value *toLL(llvm::Module *M) override;
protected:
  void print(std::ostream &Stream) const override;
};

class ReturnInst : public Instruction {
public:
  ReturnInst(Type *Ty, bool IsNotVoid);
  virtual ~ReturnInst() {}

  /// Really N can either be 0 or 1; We don't do funky multi-output functions
  void *operator new(size_t S, unsigned N);

  static ReturnInst *get(Value *V, Context *K);
  static bool classof(const Value *V);

  /// For single operand functions, {get,set}Val serves an obvious purpose
  Value *getVal();
  void setVal(Value *V);

  virtual llvm::Value *toLL(llvm::Module *M) override;
protected:
  void print(std::ostream &Stream) const override;
};

class IfInst : public Instruction {
public:
  IfInst(Type *Ty);
  virtual ~IfInst() {}

  /// Constant 3 operands
  void *operator new(size_t S);
  static IfInst *get(Value *Conditional, BasicBlock *TrueBB,
                     BasicBlock *FalseBB, Context *K);

  static bool classof(const Value *V);

  /// Getters for the 3 operands, in order
  Value *getConditional() const;
  BasicBlock *getTrueBB() const;
  BasicBlock *getFalseBB() const;

  /// Picks up the ball after the phi statement to toLL() the rest of the
  /// function
  virtual llvm::Value *toLL(llvm::Module *M) override;
protected:
  void print(std::ostream &Stream) const override;
};
}

#endif
