//-*- C++ -*-

#ifndef TYPE_H
#define TYPE_H

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
};

class Type : public FoldingSetNode {
protected:
  location SourceLoc;
public:
  Type(RTType ID) : TyID(ID) {}
  RTType getTyID() const { return TyID; }
  static Type *get() = delete;
  void setSourceLocation(location SrcLoc)
  {
    SourceLoc = SrcLoc;
  }
  location getSourceLocation() {
    return SourceLoc;
  }
  friend ostream &operator<<(ostream &Stream, const Type &T) {
    T.print(Stream);
    return Stream;
  }
  virtual llvm::Type *toLL(llvm::Module *M = nullptr, Context *K = nullptr) = 0;
protected:
  virtual void print(std::ostream &Stream) const = 0;
private:
  const RTType TyID;
};

class UnType : public Type {
public:
  UnType(): Type(RT_UnType) {}
  static UnType *get(Context *K) {
    static auto UniqueUnType = new UnType();
    return UniqueUnType;
  }
  static bool classof(const Type *T) {
    return T->getTyID() == RT_UnType;
  }
  friend ostream &operator<<(ostream &Stream, const UnType &U) {
    U.print(Stream);
    return Stream;
  }
  virtual llvm::Type *toLL(llvm::Module *M = nullptr, Context *K = nullptr);
protected:
  virtual void print(std::ostream &Stream) const {
    Stream << "UnType";
  }
};

class VoidType : public Type {
public:
  VoidType(): Type(RT_VoidType) {}
  static VoidType *get(Context *K) {
    static auto UniqueVoidType = new VoidType();
    return UniqueVoidType;
  }
  static bool classof(const Type *T) {
    return T->getTyID() == RT_VoidType;
  }
  friend ostream &operator<<(ostream &Stream, const VoidType &V) {
    V.print(Stream);
    return Stream;
  }
  virtual llvm::Type *toLL(llvm::Module *M = nullptr, Context *K = nullptr);
protected:
  virtual void print(std::ostream &Stream) const {
    Stream << "VoidType";
  }
};

class IntegerType : public Type {
public:
  IntegerType(unsigned Width):
      Type(RT_IntegerType), Bitwidth(Width) {}
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
  llvm::Type *toLL(llvm::Module *M = nullptr, Context *K = nullptr);
protected:
  unsigned Bitwidth;
  virtual void print(std::ostream &Stream) const {
    Stream << "Int";
  }
};

class BoolType : public Type {
public:
  BoolType(): Type(RT_BoolType) {}
  static BoolType *get(Context *K) {
    static auto UniqueBoolType = new BoolType();
    return UniqueBoolType;
  }
  static bool classof(const Type *T) {
    return T->getTyID() == RT_BoolType;
  }
  friend ostream &operator<<(ostream &Stream, const BoolType &T) {
    T.print(Stream);
    return Stream;
  }
  llvm::Type *toLL(llvm::Module *M = nullptr, Context *K = nullptr);
protected:
  virtual void print(std::ostream &Stream) const {
    Stream << "Bool";
  }
};

class FloatType : public Type {
public:
  FloatType(): Type(RT_FloatType) {}
  static FloatType *get(Context *K) {
    static auto UniqueFloatType = new FloatType();
    return UniqueFloatType;
  }
  static bool classof(const Type *T) {
    return T->getTyID() == RT_FloatType;
  }
  friend ostream &operator<<(ostream &Stream, const FloatType &T) {
    T.print(Stream);
    return Stream;
  }
  llvm::Type *toLL(llvm::Module *M = nullptr, Context *K = nullptr);
protected:
  virtual void print(std::ostream &Stream) const {
    Stream << "Float";
  }
};

class StringType : public Type {
public:
  StringType(): Type(RT_StringType) {}
  static StringType *get(Context *K) {
    static auto UniqueStringType = new StringType();
    return UniqueStringType;
  }
  static bool classof(const Type *T) {
    return T->getTyID() == RT_StringType;
  }
  friend ostream &operator<<(ostream &Stream, const StringType &T) {
    T.print(Stream);
    return Stream;
  }
  llvm::Type *toLL(llvm::Module *M = nullptr, Context *K = nullptr);
protected:
  virtual void print(std::ostream &Stream) const {
    Stream << "String";
  }
};

class FunctionType : public Type {
  Type *ReturnType;
  std::vector<Type *> ArgumentTypes;
public:
  template <typename R>
  FunctionType(R RTy, std::vector<Type *> ATys) :
      Type(RT_FunctionType), ReturnType(RTy), ArgumentTypes(ATys) {}
  template <typename R>
  static FunctionType *get(R RTy, std::vector<Type *> ATys, Context *K) {
    FoldingSetNodeID ID;
    void *IP;
    FunctionType::Profile(ID, RTy, ATys);
    if (auto FTy = K->FTyCache.FindNodeOrInsertPos(ID, IP))
      return FTy;
    FunctionType *FTy = new (K->RhAllocator) FunctionType(RTy, ATys);
    K->FTyCache.InsertNode(FTy, IP);
    return FTy;
  }
  template <typename R, typename... As>
  static FunctionType *get(R RTy, As... ATys, Context *K) {
    return FunctionType::get(RTy, {ATys...}, K);
  }
  static bool classof(const Type *T) {
    return T->getTyID() == RT_FunctionType;
  }
  static inline void Profile(FoldingSetNodeID &ID, const Type *RTy,
                             const std::vector<Type *> ATys) {
    ID.AddPointer(RTy);
    for (auto &T: ATys)
      ID.AddPointer(T);
  }
  void Profile(FoldingSetNodeID &ID) {
    Profile(ID, ReturnType, ArgumentTypes);
  }
  Type *getATy(unsigned i) {
    return ArgumentTypes[i];
  }
  Type *getRTy() {
    return ReturnType;
  }
  void setRTy(Type *T) {
    ReturnType = T;
  }
  std::vector<Type *> getATys() {
    return ArgumentTypes;
  }
  friend ostream &operator<<(ostream &Stream, const FunctionType &T) {
    T.print(Stream);
    return Stream;
  }
  llvm::Type *toLL(llvm::Module *M = nullptr, Context *K = nullptr);
protected:
  virtual void print(std::ostream &Stream) const {
    Stream << "Fn(";
    if (ArgumentTypes.size()) {
      Stream << *ArgumentTypes[0];
      for (auto ATy = std::next(std::begin(ArgumentTypes));
           ATy != std::end(ArgumentTypes); ++ATy)
        Stream << ", " << *ATy;
    } else
      Stream << "VoidType";
    if (ReturnType)
      Stream << " -> " << *ReturnType << ")";
    else
      Stream << " -> ())";
  }
};
}

#endif
