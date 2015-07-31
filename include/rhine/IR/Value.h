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
  RT_Argument,
  RT_GlobalString,
  RT_ConstantInt,
  RT_ConstantBool,
  RT_ConstantFloat,
  RT_Function,
  RT_BasicBlock,
  RT_AddInst,
  RT_CallInst,
  RT_MallocInst,
  RT_LoadInst,
  RT_IfInst,
};

class Value : public FoldingSetNode {
protected:
  Type *VTy;
  location SourceLoc;
  std::string Name;
public:
  Value(Type *VTy, RTValue ID, std::string N = "");
  virtual ~Value() { }
  Value *get() = delete;
  void setSourceLocation(location SrcLoc);
  location getSourceLocation();
  RTValue getValID() const;
  Type *getType() const;
  void setType(Type *T);
  std::string getName() const;
  void setName(std::string Str);
  virtual llvm::Value *toLL(llvm::Module *M, Context *K) = 0;
  friend ostream &operator<<(ostream &Stream, const Value &V) {
    V.print(Stream);
    return Stream;
  }
  void dump();
protected:
  virtual void print(std::ostream &Stream) const = 0;
private:
  const RTValue ValID;
};

class Argument : public Value {
public:
  Argument(std::string N, Type *T);
  static Argument *get(std::string N, Type *T, Context *K);
  static bool classof(const Value *V);
  llvm::Value *toLL(llvm::Module *M, Context *K);
  friend ostream &operator<<(ostream &Stream, const Argument &S) {
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
