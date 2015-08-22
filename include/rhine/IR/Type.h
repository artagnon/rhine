//-*- C++ -*-

#ifndef RHINE_TYPE_H
#define RHINE_TYPE_H

#include "llvm/IR/Module.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/TypeBuilder.h"
#include "llvm/IR/Constants.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/STLExtras.h"

#include <string>
#include <vector>
#include <sstream>

#include "location.hh"

using namespace std;
using namespace llvm;

namespace rhine {
class Context;

enum RTType {
  RT_UnType,
  RT_VoidType,
  RT_IntegerType,
  RT_BoolType,
  RT_FloatType,
  RT_StringType,
  RT_FunctionType,
  RT_PointerType,
};

class Type : public FoldingSetNode {
protected:
  Context *Kontext;
  location SourceLoc;
  Type(RTType ID);
public:
  Type(Context *K, RTType ID);
  virtual ~Type();
  RTType getTyID() const;
  static Type *get() = delete;
  Context *getContext();
  void setSourceLocation(location SrcLoc);
  location getSourceLocation();
  friend ostream &operator<<(ostream &Stream, const Type &T) {
    T.print(Stream);
    return Stream;
  }
  void dump();
  virtual llvm::Type *toLL(llvm::Module *M) = 0;
protected:
  virtual void print(std::ostream &Stream) const = 0;
private:
  const RTType TyID;
  friend class Context;
};

class UnType : public Type {
  UnType();
public:
  virtual ~UnType();
  static UnType *get(Context *K);
  static bool classof(const Type *T);
  virtual llvm::Type *toLL(llvm::Module *M) override;
protected:
  virtual void print(std::ostream &Stream) const override;
  friend class Context;
};

class VoidType : public Type {
  VoidType();
public:
  virtual ~VoidType();
  static VoidType *get(Context *K);
  static bool classof(const Type *T);
  virtual llvm::Type *toLL(llvm::Module *M) override;
protected:
  virtual void print(std::ostream &Stream) const override;
  friend class Context;
};

class IntegerType : public Type {
public:
  IntegerType(Context *K, unsigned Width);
  virtual ~IntegerType();
  static IntegerType *get(unsigned Bitwidth, Context *K);
  unsigned getBitwidth();
  static bool classof(const Type *T);
  static inline void Profile(FoldingSetNodeID &ID, const unsigned &W);
  void Profile(FoldingSetNodeID &ID);
  virtual llvm::Type *toLL(llvm::Module *M) override;
protected:
  unsigned Bitwidth;
  virtual void print(std::ostream &Stream) const override;
};

class BoolType : public Type {
  BoolType();
public:
  virtual ~BoolType();
  static BoolType *get(Context *K);
  static bool classof(const Type *T);
  virtual llvm::Type *toLL(llvm::Module *M) override;
protected:
  virtual void print(std::ostream &Stream) const override;
  friend class Context;
};

class FloatType : public Type {
  FloatType();
public:
  virtual ~FloatType();
  static FloatType *get(Context *K);
  static bool classof(const Type *T);
  virtual llvm::Type *toLL(llvm::Module *M) override;
protected:
  virtual void print(std::ostream &Stream) const override;
  friend class Context;
};

class StringType : public Type {
  StringType();
public:
  virtual ~StringType();
  static StringType *get(Context *K);
  static bool classof(const Type *T);
  virtual llvm::Type *toLL(llvm::Module *M) override;
protected:
  virtual void print(std::ostream &Stream) const override;
  friend class Context;
};

class FunctionType : public Type {
  Type *ReturnType;
  bool VariadicFlag;
  std::vector<Type *> ArgumentTypes;
public:
  FunctionType(Context *K, Type *RTy, std::vector<Type *> ATys, bool IsV);
  virtual ~FunctionType();
  static FunctionType *get(Type *RTy, std::vector<Type *> ATys,
                           bool IsV, Context *K);
  static FunctionType *get(Type *RTy, Context *K);
  static bool classof(const Type *T);
  static inline void Profile(FoldingSetNodeID &ID, const Type *RTy,
                             const std::vector<Type *> ATys, bool IsV);
  void Profile(FoldingSetNodeID &ID);
  Type *getATy(unsigned i);
  std::vector<Type *> getATys();
  Type *getRTy();
  bool isVariadic() const;
  virtual llvm::Type *toLL(llvm::Module *M) override;
protected:
  virtual void print(std::ostream &Stream) const override;
};

class PointerType : public Type {
  Type *ContainedType;
public:
  PointerType(Context *K, Type *CTy);
  virtual ~PointerType();
  static PointerType *get(Type *CTy, Context *K);
  static bool classof(const Type *T);
  static inline void Profile(FoldingSetNodeID &ID, const Type *CTy);
  void Profile(FoldingSetNodeID &ID);
  Type *getCTy();
  virtual llvm::Type *toLL(llvm::Module *M) override;
protected:
  virtual void print(std::ostream &Stream) const override;
};
}

#endif
