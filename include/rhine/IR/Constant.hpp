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
class Context;
class Module;

class Constant : public User, public FoldingSetNode {
public:
  Constant(Type *Ty, RTValue ID, unsigned NumOps = 0, std::string N = "");
  virtual ~Constant() = default;
  static bool classof(const Value *V);
  virtual llvm::Constant *toLL(llvm::Module *M) = 0;

protected:
  virtual void print(DiagnosticPrinter &Stream) const = 0;
};

class ConstantInt : public Constant {
  int Val;

public:
  ConstantInt(int Val, unsigned Bitwidth, Context *K);
  virtual ~ConstantInt() = default;
  void *operator new(size_t s);
  static ConstantInt *get(int Val, unsigned Bitwidth, Context *K);
  IntegerType *type() const;
  static bool classof(const Value *V);
  int val() const;
  unsigned getBitwidth() const;
  static inline void Profile(FoldingSetNodeID &ID, const Type *Ty,
                             const int &Val);
  void Profile(FoldingSetNodeID &ID) const;
  virtual llvm::Constant *toLL(llvm::Module *M) override;

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
  virtual llvm::Constant *toLL(llvm::Module *M) override;

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
  virtual llvm::Constant *toLL(llvm::Module *M) override;

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
  virtual llvm::Constant *toLL(llvm::Module *M) override;

protected:
  virtual void print(DiagnosticPrinter &Stream) const override;
};
}
