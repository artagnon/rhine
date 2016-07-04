#pragma once

#include "llvm/IR/Value.h"

#include <sstream>
#include <string>
#include <vector>

#include "rhine/ADT/IList.hpp"
#include "rhine/IR/User.hpp"
#include "rhine/IR/Value.hpp"

namespace rhine {
class Use;
class Type;
class Context;
class BasicBlock;
class FunctionType;

class Instruction : public User, public IListNode<Instruction> {
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
  virtual void print(DiagnosticPrinter &Stream) const = 0;
};

class BinaryArithInst : public Instruction {
public:
  /// We can explicitly request a particular type from two possibly-different
  /// types being +, -, *, or /'ded.
  BinaryArithInst(RTValue InstSelector, Type *Ty, Value *Op0, Value *Op1);

  virtual ~BinaryArithInst() {}

  /// Allocate a constant two operands
  void *operator new(size_t S);

  /// Context inferred from Op0
  static BinaryArithInst *get(RTValue InstSelector, Value *Op0, Value *Op1);
  static bool classof(const Value *V);
  virtual llvm::Value *toLL(llvm::Module *M) override;

protected:
  void print(DiagnosticPrinter &Stream) const override;
};

class CallInst : public Instruction {
public:
  CallInst(Type *Ty, unsigned NumOps, std::string N);
  virtual ~CallInst() = default;

  /// N operands, including Callee.
  void *operator new(size_t S, unsigned N);

  /// Set all the operands here
  static CallInst *get(Value *Callee, std::vector<Value *> Ops);

  /// Custom RTTI support
  static bool classof(const Value *V);

  /// Get the type of the underlying function
  FunctionType *getFTy() const;

  /// Get the argument types of the underlying function
  std::vector<Type *> getATys() const;

  /// Get the return type of the whole instruction (the underlying function)
  virtual Type *returnType() const override;

  /// Get the underlying function that is called
  Value *getCallee() const;

  /// There should be little cleverness in lowering
  virtual llvm::Value *toLL(llvm::Module *M) override;

protected:
  void print(DiagnosticPrinter &Stream) const override;
};

class AbstractBindInst : public Instruction {
public:
  AbstractBindInst(RTValue RTVal, std::string N, Value *V);
  virtual ~AbstractBindInst() = default;
  static bool classof(const Value *V);

  /// Operand0 manipulators
  virtual Value *val() = 0;
  virtual void setVal(Value *V) = 0;

  virtual llvm::Value *toLL(llvm::Module *M) override = 0;
};

class BindInst : public AbstractBindInst {
public:
  BindInst(std::string N, Value *V);
  virtual ~BindInst() = default;
  void *operator new(size_t S);
  static BindInst *get(std::string N, Value *V);
  static bool classof(const Value *V);

  virtual Value *val() override;
  virtual void setVal(Value *V) override;

  virtual llvm::Value *toLL(llvm::Module *M) override;

protected:
  void print(DiagnosticPrinter &Stream) const override;
};

class MallocInst : public AbstractBindInst {
public:
  MallocInst(std::string N, Value *V);
  virtual ~MallocInst() = default;
  void *operator new(size_t S);
  static MallocInst *get(std::string N, Value *V);
  static bool classof(const Value *V);

  virtual Value *val() override;
  virtual void setVal(Value *V) override;

  virtual llvm::Value *toLL(llvm::Module *M) override;

protected:
  void print(DiagnosticPrinter &Stream) const override;
};

class LoadInst : public Instruction {
public:
  /// Load an existing MallocInst
  LoadInst(MallocInst *M);

  /// Noop.
  virtual ~LoadInst();
  void *operator new(size_t S);
  static LoadInst *get(MallocInst *M);
  Value *val() const;
  static bool classof(const Value *V);
  virtual llvm::Value *toLL(llvm::Module *M) override;

protected:
  void print(DiagnosticPrinter &Stream) const override;
};

class StoreInst : public Instruction {
public:
  /// A socket and plug; they better fit well
  StoreInst(Value *MallocedValue, Value *NewValue);

  /// Noop.
  virtual ~StoreInst();

  /// Allocate a constant two operands
  void *operator new(size_t S);

  static StoreInst *get(Value *MallocedValue, Value *NewValue);

  /// Get operands 0 and 1 respectively
  Value *getMallocedValue() const;
  Value *getNewValue() const;

  static bool classof(const Value *V);
  virtual llvm::Value *toLL(llvm::Module *M) override;

protected:
  void print(DiagnosticPrinter &Stream) const override;
};

/// Can only be used to terminate functions: has either 0 or 1 arguments. Branch
/// instructions require to come together at phi nodes.
class ReturnInst : public Instruction {
public:
  ReturnInst(Type *Ty, bool IsVoid);
  virtual ~ReturnInst();

  /// Really N can either be 0 or 1; We don't do funky multi-output functions
  void *operator new(size_t S, unsigned N);

  static ReturnInst *get(Value *V, Context *K);
  static bool classof(const Value *V);

  /// For single operand functions, {get,set}Val serves an obvious purpose
  Value *val();
  void setVal(Value *V);

  virtual llvm::Value *toLL(llvm::Module *M) override;

protected:
  void print(DiagnosticPrinter &Stream) const override;
};

/// Used to communicate unifying phi values from the end of a branch
/// instruction.
/// if true do
///   2 <-- TerminatorInst
/// else ...
///
class TerminatorInst : public Instruction {
public:
  TerminatorInst(Type *Ty);
  virtual ~TerminatorInst();

  /// Fixed at one operand.
  void *operator new(size_t S);

  static TerminatorInst *get(Value *V);
  static bool classof(const Value *V);

  /// For single operand functions, {get,set}Val serves an obvious purpose
  Value *val();
  void setVal(Value *V);

  /// Codegen to the contained value directly.
  virtual llvm::Value *toLL(llvm::Module *M) override;

protected:
  void print(DiagnosticPrinter &Stream) const override;
};

class IfInst : public Instruction {
public:
  IfInst(Type *Ty);
  virtual ~IfInst();

  /// Constant 3 operands
  void *operator new(size_t S);
  static IfInst *get(Value *Conditional, BasicBlock *TrueBB,
                     BasicBlock *FalseBB);

  static bool classof(const Value *V);

  /// Getters for the 3 operands, in order
  Value *getConditional() const;
  BasicBlock *getTrueBB() const;
  BasicBlock *getFalseBB() const;

  /// Codegens until the phi node, and returns the value to optionally use it in
  /// an assignment.
  virtual llvm::Value *toLL(llvm::Module *M) override;

protected:
  void print(DiagnosticPrinter &Stream) const override;
};

class IndexingInst : public Instruction {
public:
  /// NumOps is the number of operands excluding the Value being indexed into
  IndexingInst(Type *Ty, unsigned NumOps);

  /// Noop
  virtual ~IndexingInst();

  /// N is the total number of operands, including Value being indexed into
  void *operator new(size_t S, unsigned N);

  /// V must eventually be resolved to a BindInst
  static IndexingInst *get(Value *V, std::vector<size_t> &Idxes);

  /// Eventually, all Idxes must resolve to Integers
  static IndexingInst *get(Value *V, std::vector<Value *> &Idxes);

  /// Just itself
  static bool classof(const Value *V);

  /// Alias for getOperand(0)
  Value *val() const;

  /// Alias for getOperands()
  std::vector<Value *> getIndices() const;

  /// Somewhat non-trivial
  virtual llvm::Value *toLL(llvm::Module *M) override;

protected:
  void print(DiagnosticPrinter &Stream) const override;
};

typedef IPList<Instruction> InstListType;
typedef InstListType::iterator inst_iterator;
}
