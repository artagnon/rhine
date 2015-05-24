//-*- C++ -*-
#ifndef RHINE_VALUE_H
#define RHINE_VALUE_H

#include "llvm/IR/Value.h"

#include <string>
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
  Type *VTy;
  location SourceLoc;
public:
  Value(Type *VTy, RTValue ID);
  virtual ~Value() { }
  Value *get() = delete;
  void setSourceLocation(location SrcLoc);
  location getSourceLocation();
  RTValue getValID() const;
  Type *getType() const;
  void setType(Type *T);
  virtual Type *typeInfer(Context *K = nullptr) = 0;
  virtual llvm::Value *toLL(llvm::Module *M, Context *K) = 0;
  friend ostream &operator<<(ostream &Stream, const Value &V) {
    V.print(Stream);
    return Stream;
  }
protected:
  virtual void print(std::ostream &Stream) const = 0;
private:
  const RTValue ValID;
};

class Symbol : public Value {
  std::string Name;
public:
  Symbol(std::string N, Type *T);
  static Symbol *get(std::string N, Type *T, Context *K);
  static bool classof(const Value *V);
  static inline void Profile(FoldingSetNodeID &ID,
                             const std::string &N, const Type *T);
  void Profile(FoldingSetNodeID &ID);
  std::string getName();
  Type *typeInfer(Context *K = nullptr);
  llvm::Value *toLL(llvm::Module *M, Context *K);
  friend ostream &operator<<(ostream &Stream, const Symbol &S) {
    S.print(Stream);
    return Stream;
  }
protected:
  virtual void print(std::ostream &Stream) const;
};

class GlobalString : public Value {
  std::string Val;
public:
  GlobalString(std::string Val, Context *K);
  static GlobalString *get(std::string Val, Context *K);
  static bool classof(const Value *V);
  std::string getVal();
  Type *typeInfer(Context *K = nullptr);
  llvm::Value *toLL(llvm::Module *M, Context *K);
  friend ostream &operator<<(ostream &Stream, const GlobalString &S) {
    S.print(Stream);
    return Stream;
  }
protected:
  virtual void print(std::ostream &Stream) const;
};
}

#endif
