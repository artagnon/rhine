#pragma once

#include "llvm/ADT/iterator_range.h"
#include "llvm/IR/Constants.h"

#include <sstream>
#include <string>
#include <vector>

#include "rhine/IR/BasicBlock.hpp"
#include "rhine/IR/Type.hpp"
#include "rhine/IR/UnresolvedValue.hpp"
#include "rhine/IR/User.hpp"

using namespace llvm;

namespace rhine {
class Bytecode;
class Context;
class Module;

class Constant : public User, public FoldingSetNode {
public:
  Constant(Type *Ty, RTValue ID, unsigned NumOps = 0, std::string N = "");
  virtual ~Constant() = default;
  static bool classof(const Value *V);
  virtual llvm::Constant *generate(llvm::Module *M) = 0;
  virtual void generate(Bytecode *B) = 0;

protected:
  virtual void print(DiagnosticPrinter &Stream) const = 0;
};

class ConstantInt : public Constant {
  int Val;

public:
  /// For predictability, use only Bitwidths of powers of 2.
  ConstantInt(int Val, unsigned Bitwidth, Context *K);

  /// Nothing special.
  virtual ~ConstantInt() = default;

  /// Asks for memory from User.
  void *operator new(size_t s);

  /// The standard way to get a new instance.
  static ConstantInt *get(int Val, unsigned Bitwidth, Context *K);

  /// Get the unitary instance of IntegerType, of this bitwidth.
  IntegerType *type() const;

  /// RT_ConstantInt.
  static bool classof(const Value *V);

  /// The underlying int.
  int val() const;

  /// Ask for my width (why?)
  unsigned bitwidth() const;

  /// Used to uniquify Constants into Context.
  static inline void Profile(FoldingSetNodeID &ID, const Type *Ty,
                             const int &Val);
  void Profile(FoldingSetNodeID &ID) const;

  /// The most straightforward lowerings.
  llvm::Constant *generate(llvm::Module *M) override;
  void generate(Bytecode *B) override;

protected:
  virtual void print(DiagnosticPrinter &Stream) const override;
};

class ConstantBool : public Constant {
  bool Val;

public:
  ConstantBool(bool Val, Context *K);
  virtual ~ConstantBool() = default;
  void *operator new(size_t s);
  static ConstantBool *get(bool Val, Context *K);
  BoolType *type() const;
  static bool classof(const Value *V);
  float val() const;
  static inline void Profile(FoldingSetNodeID &ID, const Type *Ty,
                             const bool &Val);
  void Profile(FoldingSetNodeID &ID) const;

  /// The most straightforward lowerings.
  llvm::Constant *generate(llvm::Module *M) override;
  void generate(Bytecode *B) override;

protected:
  virtual void print(DiagnosticPrinter &Stream) const override;
};

class ConstantFloat : public Constant {
  float Val;

public:
  ConstantFloat(float Val, Context *K);
  virtual ~ConstantFloat() = default;
  void *operator new(size_t s);
  static ConstantFloat *get(float Val, Context *K);
  FloatType *type() const;
  static bool classof(const Value *V);
  float val() const;
  static inline void Profile(FoldingSetNodeID &ID, const Type *Ty,
                             const float &Val);
  void Profile(FoldingSetNodeID &ID) const;

  /// As many lowering codepaths as there are.
  llvm::Constant *generate(llvm::Module *M) override;
  void generate(Bytecode *B) override;

protected:
  virtual void print(DiagnosticPrinter &Stream) const override;
};

class Pointer : public Constant {
  Value *Val;

public:
  Pointer(Value *V, Type *Ty);
  virtual ~Pointer() = default;
  void *operator new(size_t s);
  static Pointer *get(Value *V);
  PointerType *type() const;
  static bool classof(const Value *V);
  void setVal(Value *V);
  Value *val() const;
  static inline void Profile(FoldingSetNodeID &ID, const Type *Ty,
                             const Value *Val);
  void Profile(FoldingSetNodeID &ID) const;

  /// As many lowering codepaths as there are.
  llvm::Constant *generate(llvm::Module *M) override;
  void generate(Bytecode *B) override;

protected:
  virtual void print(DiagnosticPrinter &Stream) const override;
};
}
