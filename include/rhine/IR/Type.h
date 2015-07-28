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
#include "rhine/Context.h"

using namespace std;
using namespace llvm;

namespace rhine {
// Discriminator for LLVM-style RTTI (dyn_cast<> et al.)
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
  location SourceLoc;
public:
  Type(RTType ID) : TyID(ID) {}
  virtual ~Type() {}
  RTType getTyID() const;
  static Type *get() = delete;
  void setSourceLocation(location SrcLoc);
  location getSourceLocation();
  friend ostream &operator<<(ostream &Stream, const Type &T) {
    T.print(Stream);
    return Stream;
  }
  void dump();
  virtual llvm::Type *toLL(llvm::Module *M, Context *K) = 0;
protected:
  virtual void print(std::ostream &Stream) const = 0;
private:
  const RTType TyID;
};

class UnType : public Type {
public:
  UnType(): Type(RT_UnType) {}
  virtual ~UnType() {}
  static UnType *get(Context *K) {
    static auto UniqueUnType = new UnType;
    return UniqueUnType;
  }
  static bool classof(const Type *T) {
    return T->getTyID() == RT_UnType;
  }
  friend ostream &operator<<(ostream &Stream, const UnType &U) {
    U.print(Stream);
    return Stream;
  }
  virtual llvm::Type *toLL(llvm::Module *M, Context *K);
protected:
  virtual void print(std::ostream &Stream) const {
    Stream << "UnType";
  }
};

class VoidType : public Type {
public:
  VoidType(): Type(RT_VoidType) {}
  virtual ~VoidType() {}
  static VoidType *get(Context *K) {
    static auto UniqueVoidType = new VoidType;
    return UniqueVoidType;
  }
  static bool classof(const Type *T) {
    return T->getTyID() == RT_VoidType;
  }
  friend ostream &operator<<(ostream &Stream, const VoidType &V) {
    V.print(Stream);
    return Stream;
  }
  virtual llvm::Type *toLL(llvm::Module *M, Context *K);
protected:
  virtual void print(std::ostream &Stream) const {
    Stream << "()";
  }
};

class IntegerType : public Type {
public:
  IntegerType(unsigned Width):
      Type(RT_IntegerType), Bitwidth(Width) {}
  virtual ~IntegerType() {}
  static IntegerType *get(unsigned Bitwidth, Context *K) {
    FoldingSetNodeID ID;
    void *IP;
    IntegerType::Profile(ID, Bitwidth);
    if (auto T = K->ITyCache.FindNodeOrInsertPos(ID, IP))
      return T;
    auto T = new (K->RhAllocator) IntegerType(Bitwidth);
    K->ITyCache.InsertNode(T, IP);
    return T;
  }
  unsigned getBitwidth() {
    return Bitwidth;
  }
  static bool classof(const Type *T) {
    return T->getTyID() == RT_IntegerType;
  }
  static inline void Profile(FoldingSetNodeID &ID, const unsigned &W) {
    ID.AddInteger(W);
  }
  void Profile(FoldingSetNodeID &ID) {
    Profile(ID, Bitwidth);
  }
  friend ostream &operator<<(ostream &Stream, const IntegerType &T) {
    T.print(Stream);
    return Stream;
  }
  llvm::Type *toLL(llvm::Module *M, Context *K);
protected:
  unsigned Bitwidth;
  virtual void print(std::ostream &Stream) const {
    Stream << "Int";
  }
};

class BoolType : public Type {
public:
  BoolType(): Type(RT_BoolType) {}
  virtual ~BoolType() {}
  static BoolType *get(Context *K) {
    static auto UniqueBoolType = new BoolType;
    return UniqueBoolType;
  }
  static bool classof(const Type *T) {
    return T->getTyID() == RT_BoolType;
  }
  friend ostream &operator<<(ostream &Stream, const BoolType &T) {
    T.print(Stream);
    return Stream;
  }
  llvm::Type *toLL(llvm::Module *M, Context *K);
protected:
  virtual void print(std::ostream &Stream) const {
    Stream << "Bool";
  }
};

class FloatType : public Type {
public:
  FloatType(): Type(RT_FloatType) {}
  virtual ~FloatType() {}
  static FloatType *get(Context *K) {
    static auto UniqueFloatType = new FloatType;
    return UniqueFloatType;
  }
  static bool classof(const Type *T) {
    return T->getTyID() == RT_FloatType;
  }
  friend ostream &operator<<(ostream &Stream, const FloatType &T) {
    T.print(Stream);
    return Stream;
  }
  llvm::Type *toLL(llvm::Module *M, Context *K);
protected:
  virtual void print(std::ostream &Stream) const {
    Stream << "Float";
  }
};

class StringType : public Type {
public:
  StringType(): Type(RT_StringType) {}
  virtual ~StringType() {}
  static StringType *get(Context *K) {
    static auto UniqueStringType = new StringType;
    return UniqueStringType;
  }
  static bool classof(const Type *T) {
    return T->getTyID() == RT_StringType;
  }
  friend ostream &operator<<(ostream &Stream, const StringType &T) {
    T.print(Stream);
    return Stream;
  }
  llvm::Type *toLL(llvm::Module *M, Context *K);
protected:
  virtual void print(std::ostream &Stream) const {
    Stream << "String";
  }
};

class FunctionType : public Type {
  Type *ReturnType;
  bool VariadicFlag;
  std::vector<Type *> ArgumentTypes;
public:
  FunctionType(Type *RTy, std::vector<Type *> ATys, bool IsV);
  virtual ~FunctionType() {}
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
  friend ostream &operator<<(ostream &Stream, const FunctionType &T) {
    T.print(Stream);
    return Stream;
  }
  llvm::Type *toLL(llvm::Module *M, Context *K);
protected:
  virtual void print(std::ostream &Stream) const;
};

class PointerType : public Type {
  Type *ContainedType;
public:
  PointerType(Type *CTy) : Type(RT_PointerType), ContainedType(CTy) {}
  virtual ~PointerType() {}
  static PointerType *get(Type *CTy, Context *K) {
    FoldingSetNodeID ID;
    void *IP;
    PointerType::Profile(ID, CTy);
    if (auto PTy = K->PTyCache.FindNodeOrInsertPos(ID, IP))
      return PTy;
    PointerType *PTy = new (K->RhAllocator) PointerType(CTy);
    K->PTyCache.InsertNode(PTy, IP);
    return PTy;
  }
  static bool classof(const Type *T) {
    return T->getTyID() == RT_PointerType;
  }
  static inline void Profile(FoldingSetNodeID &ID, const Type *CTy) {
    ID.AddPointer(CTy);
  }
  void Profile(FoldingSetNodeID &ID) {
    Profile(ID, ContainedType);
  }
  Type *getCTy() {
    return ContainedType;
  }
  friend ostream &operator<<(ostream &Stream, const PointerType &T) {
    T.print(Stream);
    return Stream;
  }
  llvm::Type *toLL(llvm::Module *M, Context *K);
protected:
  virtual void print(std::ostream &Stream) const {
    Stream << *ContainedType << "*";
  }
};
}

#endif
