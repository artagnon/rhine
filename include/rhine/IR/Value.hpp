#pragma once

#include "llvm/IR/Module.h"
#include "llvm/IR/Value.h"

#include <sstream>
#include <string>

#include "rhine/Diagnostic/Diagnostic.hpp"
#include "rhine/Parse/Parser.hpp"

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

/// Used by Lowering instruments to generate a new Value and cache it.
#define returni(ToCompute)                                                     \
  {                                                                            \
    auto Computed = ToCompute;                                                 \
    setLoweredValue(Computed);                                                 \
    return Computed;                                                           \
  }

/// Used by Lowering instruments to check cached Value.
#define CHECK_LoweredValue                                                     \
  {                                                                            \
    if (LoweredValue) {                                                        \
      return LoweredValue;                                                     \
    }                                                                          \
  }

class Value {
protected:
  Type *VTy;
  Use *UseList;
  Location SourceLoc;
  std::string Name;
  llvm::Value *LoweredValue;

public:
  Value(Type *VTy, RTValue ID, std::string N = "");
  virtual ~Value();
  Value *get() = delete;
  static bool classof(const Value *V);
  Context *context();
  void setSourceLocation(Location SrcLoc);

  /// Every node has some location in the original source literal.
  Location sourceLocation();

  /// Required to check class without casting.
  RTValue op() const;
  Type *getType() const;
  llvm::Value *getLoweredValue() const;
  void setLoweredValue(llvm::Value *V);

  /// Get the return type of the Value. For most Values, this is just the type.
  /// CallInst, Function are notable exceptions. Useful in places that just need
  /// to reason about chained expressions.
  virtual Type *getRTy() const;

  /// Setting VTy is necessary in some early Transforms (type inference, type
  /// coercion).
  void setType(Type *T);

  /// Does VTy->getTyID equal RT_UnType?
  bool isUnTyped();

  /// This is the SSA name.
  std::string getName() const;

  /// You can set the name too.
  void setName(std::string Str);

  /// Type cannot be lowered in itself.
  virtual llvm::Value *toLL(llvm::Module *M) = 0;

  /// Stuff coming in from a DiagnosticPrinter object.
  friend DiagnosticPrinter &operator<<(DiagnosticPrinter &Stream,
                                       const Value &V) {
    V.print(Stream);
    return Stream;
  }

  /// Stuff coming in from an ostream object.
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
