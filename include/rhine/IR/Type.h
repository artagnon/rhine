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
  RT_PointerType,
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
  virtual llvm::Type *toLL(llvm::Module *M, Context *K) = 0;
protected:
  virtual void print(std::ostream &Stream) const = 0;
private:
  const RTType TyID;
};

class UnType : public Type {
public:
  UnType(): Type(RT_UnType) {}
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
  std::vector<Type *> ArgumentTypes;
public:
  FunctionType(Type *RTy, std::vector<Type *> ATys) :
      Type(RT_FunctionType), ReturnType(RTy), ArgumentTypes(ATys) {}
  static FunctionType *get(Type *RTy, std::vector<Type *> ATys, Context *K) {
    FoldingSetNodeID ID;
    void *IP;
    FunctionType::Profile(ID, RTy, ATys);
    if (auto FTy = K->FTyCache.FindNodeOrInsertPos(ID, IP))
      return FTy;
    FunctionType *FTy = new (K->RhAllocator) FunctionType(RTy, ATys);
    K->FTyCache.InsertNode(FTy, IP);
    return FTy;
  }
  static FunctionType *get(Type *RTy, Context *K) {
    return FunctionType::get(RTy, { VoidType::get(K) }, K);
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
    assert(i < ArgumentTypes.size() && "getATy() out of bounds");
    return ArgumentTypes[i];
  }
  std::vector<Type *> getATys() {
    return ArgumentTypes;
  }
  Type *getRTy() {
    return ReturnType;
  }
  friend ostream &operator<<(ostream &Stream, const FunctionType &T) {
    T.print(Stream);
    return Stream;
  }
  llvm::Type *toLL(llvm::Module *M, Context *K);
protected:
  virtual void print(std::ostream &Stream) const {
    Stream << "Fn(" << *ArgumentTypes[0];
    for (auto ATy = std::next(std::begin(ArgumentTypes));
         ATy != std::end(ArgumentTypes); ++ATy)
      Stream << " -> " << **ATy;
    Stream << " -> " << *ReturnType << ")";
  }
};

class PointerType : public Type {
  Type *ContainedType;
public:
  PointerType(Type *CTy) : Type(RT_PointerType), ContainedType(CTy) {}
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
    Stream << "Pointer(" << *ContainedType << ")";
  }
};
}

#endif
