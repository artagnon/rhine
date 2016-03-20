#ifndef RHINE_VALUE_H
#define RHINE_VALUE_H

#include "llvm/IR/Value.h"
#include "llvm/IR/Module.h"

#include <string>
#include <sstream>

#include "rhine/Parse/Parser.hpp"
#include "rhine/Diagnostic/Diagnostic.hpp"

using namespace std;
using namespace llvm;
using Location = rhine::Parser::Location;

namespace rhine {
class Use;
class User;
class Type;
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
  RT_Tensor,
  RT_Prototype,
  RT_Function,
  RT_Pointer,
  RT_Instruction,
  RT_AddInst,
  RT_SubInst,
  RT_MulInst,
  RT_DivInst,
  RT_CallInst,
  RT_BindInst,
  RT_MallocInst,
  RT_LoadInst,
  RT_StoreInst,
  RT_ReturnInst,
  RT_TerminatorInst,
  RT_IfInst,
  RT_IndexingInst,
  RT_BasicBlock,
};

class Value {
protected:
  Type *VTy;
  Use *UseList;
  Location SourceLoc;
  std::string Name;
public:
  Value(Type *VTy, RTValue ID, std::string N = "");
  virtual ~Value();
  Value *get() = delete;
  static bool classof(const Value *V);
  Context *getContext();
  void setSourceLocation(Location SrcLoc);
  Location getSourceLocation();
  RTValue getValID() const;
  Type *getType() const;

  /// Get the return type of the Value. For most Values, this is just the type.
  /// CallInst, Function are notable exceptions. Useful in places that just need
  /// to reason about chained expressions.
  virtual Type *getRTy() const;

  void setType(Type *T);
  bool isUnTyped();
  std::string getName() const;
  void setName(std::string Str);
  virtual llvm::Value *toLL(llvm::Module *M) = 0;
  friend DiagnosticPrinter &operator<<(DiagnosticPrinter &Stream, const Value &V) {
    V.print(Stream);
    return Stream;
  }
  friend ostream &operator<<(ostream &Stream, const Value &V) {
    auto DiagStream = DiagnosticPrinter(Stream);
    V.print(DiagStream);
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
  virtual void print(DiagnosticPrinter &Stream) const = 0;
private:
  const RTValue ValID;
};
}
#endif
