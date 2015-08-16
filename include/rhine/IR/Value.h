//-*- C++ -*-
#ifndef RHINE_VALUE_H
#define RHINE_VALUE_H

#include "llvm/IR/Value.h"

#include <string>
#include <sstream>

#include "location.hh"
#include "rhine/IR/Type.h"

using namespace std;
using namespace llvm;

namespace rhine {
class Use;
class User;
class Context;

enum RTValue {
  RT_User,
  RT_UnresolvedValue,
  RT_Argument,
  RT_GlobalString,
  RT_Constant,
  RT_ConstantInt,
  RT_ConstantBool,
  RT_ConstantFloat,
  RT_Prototype,
  RT_Function,
  RT_Instruction,
  RT_AddInst,
  RT_CallInst,
  RT_MallocInst,
  RT_LoadInst,
  RT_ReturnInst,
  RT_IfInst,
  RT_BasicBlock,
};

class Value {
protected:
  Type *VTy;
  Use *UseList;
  location SourceLoc;
  std::string Name;
public:
  Value(Type *VTy, RTValue ID, std::string N = "");
  virtual ~Value() { }
  Value *get() = delete;
  Context *getContext();
  void setSourceLocation(location SrcLoc);
  location getSourceLocation();
  RTValue getValID() const;
  Type *getType() const;
  void setType(Type *T);
  std::string getName() const;
  void setName(std::string Str);
  virtual llvm::Value *toLL(llvm::Module *M) = 0;
  friend ostream &operator<<(ostream &Stream, const Value &V) {
    V.print(Stream);
    return Stream;
  }
  operator Use *() const;
  User *getUser() const;
  void addUse(Use &U);
  bool use_empty() const;
  void replaceAllUsesWith(Value *V);
  void zapUseList();
  void dump();
protected:
  virtual void print(std::ostream &Stream) const = 0;
private:
  const RTValue ValID;
};
}
#endif
