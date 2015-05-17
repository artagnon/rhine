//-*- C++ -*-
#ifndef VALUE_H
#define VALUE_H

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
#include "rhine/IR/Type.h"

using namespace std;
using namespace llvm;

namespace rhine {
enum RTValue {
  RT_Symbol,
  RT_GlobalString,
  RT_ConstantInt,
  RT_ConstantBool,
  RT_ConstantFloat,
  RT_Function,
  RT_AddInst,
  RT_CallInst,
  RT_BindInst,
};

class Value : public FoldingSetNode {
protected:
  /// nullptr indicates VoidType
  Type *VTy;
  location SourceLoc;
public:
  Value(Type *VTy, RTValue ID) : VTy(VTy), ValID(ID) {}
  virtual ~Value() { };
  Value *get() = delete;
  void setSourceLocation(location SrcLoc) {
    SourceLoc = SrcLoc;
  }
  location getSourceLocation() {
    return SourceLoc;
  }
  RTValue getValID() const { return ValID; }
  Type *getType() const {
    return VTy;
  }
  void setType(Type *T) {
    VTy = T;
  }
  friend ostream &operator<<(ostream &Stream, const Value &V) {
    V.print(Stream);
    return Stream;
  }
  virtual Type *typeInfer(Context *K = nullptr) = 0;
  virtual llvm::Value *toLL(llvm::Module *M = nullptr, Context *K = nullptr) = 0;
protected:
  virtual void print(std::ostream &Stream) const = 0;
private:
  const RTValue ValID;
};

class Symbol : public Value {
  std::string Name;
public:
  Symbol(std::string N, Type *T) : Value(T, RT_Symbol), Name(N) {}

  static Symbol *get(std::string N, Type *T, Context *K) {
    FoldingSetNodeID ID;
    void *IP;
    Symbol::Profile(ID, N, T);
    if (Symbol *S = K->SymbolCache.FindNodeOrInsertPos(ID, IP)) return S;
    Symbol *S = new (K->RhAllocator) Symbol(N, T);
    K->SymbolCache.InsertNode(S, IP);
    return S;
  }
  static bool classof(const Value *V) {
    return V->getValID() == RT_Symbol;
  }
  static inline void Profile(FoldingSetNodeID &ID, const std::string &N,
                             const Type *T) {
    ID.AddString(N);
    ID.AddPointer(T);
  }
  void Profile(FoldingSetNodeID &ID) {
    Profile(ID, Name, VTy);
  }
  std::string getName() {
    return Name;
  }
  friend ostream &operator<<(ostream &Stream, const Symbol &S) {
    S.print(Stream);
    return Stream;
  }
  Type *typeInfer(Context *K = nullptr);
  llvm::Value *toLL(llvm::Module *M = nullptr, Context *K = nullptr);
protected:
  virtual void print(std::ostream &Stream) const {
    Stream << Name << " ~" << *getType();
  }
};

class GlobalString : public Value {
  std::string Val;
public:
  GlobalString(std::string Val, Context *K) :
      Value(StringType::get(K), RT_GlobalString), Val(Val) {}
  static GlobalString *get(std::string Val, Context *K) {
    return new (K->RhAllocator) GlobalString(Val, K);
  }
  static bool classof(const Value *V) {
    return V->getValID() == RT_GlobalString;
  }
  std::string getVal() {
    return Val;
  }
  friend ostream &operator<<(ostream &Stream, const GlobalString &S) {
    S.print(Stream);
    return Stream;
  }
  Type *typeInfer(Context *K = nullptr);
  // Returns GEP to GlobalStringPtr, which is a Value; data itself is in
  // constant storage.
  llvm::Value *toLL(llvm::Module *M = nullptr, Context *K = nullptr);
protected:
  virtual void print(std::ostream &Stream) const {
    Stream << "\"" << Val << "\" ~" << *getType();
  }
};
}

#endif
