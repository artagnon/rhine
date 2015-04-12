//-*- C++ -*-

#ifndef AST_H
#define AST_H

#include "llvm/IR/Module.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/TypeBuilder.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Constants.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/STLExtras.h"

#include <string>
#include <vector>

#include "rhine/SymbolTable.h"

using namespace std;

namespace rhine {
using namespace llvm;

static LLVMContext &RhContext = getGlobalContext();
static IRBuilder<> RhBuilder(RhContext);

class Type : public FoldingSetNode {
public:
  static Type *get() {
    static Type UniqueType;
    return &UniqueType;
  }
  virtual ~Type() { };
  virtual llvm::Type *toLL(llvm::Module *M = nullptr, SymbolTable *K = nullptr) {
    assert(false && "Cannot toLL() without inferring type");
  }
};

class IntegerType : public Type {
public:
  static IntegerType *get() {
    static IntegerType UniqueIntegerType;
    return &UniqueIntegerType;
  }
  llvm::Type *toLL(llvm::Module *M = nullptr, SymbolTable *K = nullptr);
};

class BoolType : public Type {
public:
  static BoolType *get() {
    static BoolType UniqueBoolType;
    return &UniqueBoolType;
  }
  llvm::Type *toLL(llvm::Module *M = nullptr, SymbolTable *K = nullptr);
};

class FloatType : public Type {
public:
  static FloatType *get() {
    static FloatType UniqueFloatType;
    return &UniqueFloatType;
  }
  llvm::Type *toLL(llvm::Module *M = nullptr, SymbolTable *K = nullptr);
};

class StringType : public Type {
public:
  static StringType *get() {
    static StringType UniqueStringType;
    return &UniqueStringType;
  }
  llvm::Type *toLL(llvm::Module *M = nullptr, SymbolTable *K = nullptr);
};

class FunctionType : public Type {
  Type *ReturnType;
  std::vector<Type *> ArgumentTypes;
public:
  template <typename R, typename... As>
  FunctionType(R RTy, As... ATys) {
    ReturnType = RTy;
    ArgumentTypes = {ATys...};
  }
  template <typename R, typename... As>
  static FunctionType *get(R RTy, As... ATys) {
    static auto UniqueFunctionType = FunctionType(RTy, ATys...);
    return &UniqueFunctionType;
  }
  Type *getATy(unsigned i) {
    return ArgumentTypes[i];
  }
  llvm::Type *toLL(llvm::Module *M = nullptr, SymbolTable *K = nullptr);
};

template <typename T> class ArrayType : public Type {
public:
  T *elTy;
  llvm::Type *toLL(llvm::Module *M = nullptr, SymbolTable *K = nullptr);
};

class Value : public FoldingSetNode {
  Type *VTy;
public:
  Value(Type *VTy) : VTy(VTy) {}
  virtual ~Value() { };
  Value *get() = delete;
  Type *getType() {
    return VTy;
  }
  virtual llvm::Value *toLL(llvm::Module *M = nullptr, SymbolTable *K = nullptr) = 0;
};

class Symbol : public Value {
  std::string Name;
public:
  Symbol(std::string N, Type *T = Type::get()) : Value(T), Name(N) {}

  static Symbol *get(std::string N, Type *T = Type::get()) {
    return new Symbol(N, T);
  }
  std::string getName() {
    return Name;
  }
  llvm::Value *toLL(llvm::Module *M = nullptr, SymbolTable *K = nullptr);
};

class GlobalString : public Value {
public:
  std::string Val;
  GlobalString(std::string Val) : Value(StringType::get()), Val(Val) {}
  static GlobalString *get(std::string Val) {
    return new GlobalString(Val);
  }
  std::string getVal() {
    return Val;
  }
  // Returns GEP to GlobalStringPtr, which is a Value; data itself is in
  // constant storage.
  llvm::Value *toLL(llvm::Module *M = nullptr, SymbolTable *K = nullptr);
};

class Constant : public Value {
public:
  Constant(Type *Ty) : Value(Ty) {}
private:
  llvm::Constant *toLL(llvm::Module *M = nullptr, SymbolTable *K = nullptr) { return nullptr; }
};

class ConstantInt : public Constant {
public:
  int Val;
  ConstantInt(int Val) : Constant(IntegerType::get()), Val(Val) {}
  static ConstantInt *get(int Val) {
    return new ConstantInt(Val);
  }
  int getVal() {
    return Val;
  }
  llvm::Constant *toLL(llvm::Module *M = nullptr, SymbolTable *K = nullptr);
};

class ConstantBool : public Constant {
public:
  bool Val;
  ConstantBool(bool Val) : Constant(BoolType::get()), Val(Val) {}
  static ConstantBool *get(bool Val) {
    return new ConstantBool(Val);
  }
  float getVal() {
    return Val;
  }
  llvm::Constant *toLL(llvm::Module *M = nullptr, SymbolTable *K = nullptr);
};

class ConstantFloat : public Constant {
public:
  float Val;
  ConstantFloat(float Val) : Constant(FloatType::get()), Val(Val) {}
  static ConstantFloat *get(float Val) {
    return new ConstantFloat(Val);
  }
  float getVal() {
    return Val;
  }
  llvm::Constant *toLL(llvm::Module *M = nullptr, SymbolTable *K = nullptr);
};

class Function : public Constant {
  std::vector<Symbol *> ArgumentList;
  std::string Name;
  std::vector<Value *> Val;
public:
  Function(FunctionType *FTy) :
      Constant(FTy) {}
  ~Function() {
    for (auto i : ArgumentList)
      delete i;
    ArgumentList.clear();
    for (auto i : Val)
      delete i;
    Val.clear();
  }
  static Function *get(FunctionType *FTy) {
    return new Function(FTy);
  }
  void setName(std::string N) {
    Name = N;
  }
  void setArgumentList(std::vector<Symbol *> L) {
    ArgumentList = L;
  }
  std::vector<Symbol *> getArgumentList() {
    return ArgumentList;
  }
  void setBody(std::vector<Value *> Body) {
    Val = Body;
  }
  std::string getName() {
    return Name;
  }
  Value *getVal() {
    return Val.back();
  }
  llvm::Constant *toLL(llvm::Module *M = nullptr, SymbolTable *K = nullptr);
};

class Instruction : public Value {
  unsigned NumOperands;
  std::vector<Value *> OperandList;
public:
  Instruction(Type *Ty) :
      Value(Ty) {}
  ~Instruction() {
    for (auto i : OperandList)
      delete i;
    OperandList.clear();
  }
  void addOperand(Value *V) {
    OperandList.push_back(V);
    NumOperands++;
  }
  Value *getOperand(unsigned i) {
    return OperandList[i];
  }
private:
  llvm::Value *toLL(llvm::Module *M = nullptr, SymbolTable *K = nullptr) { return nullptr; }
};

class AddInst : public Instruction {
public:
  AddInst(Type *Ty) : Instruction(Ty) {}
  static AddInst *get(Type *Ty) {
    return new AddInst(Ty);
  }
  llvm::Value *toLL(llvm::Module *M = nullptr, SymbolTable *K = nullptr);
};

class CallInst : public Instruction {
public:
  // May be untyped
  CallInst(std::string FunctionName, Type *Ty = IntegerType::get()) :
      Instruction(Ty), Name(FunctionName) {}
  static CallInst *get(std::string FunctionName, Type *Ty = IntegerType::get()) {
    return new CallInst(FunctionName, Ty);
  }
  std::string getName() {
    return Name;
  }
  llvm::Value *toLL(llvm::Module *M = nullptr, SymbolTable *K = nullptr);
private:
  std::string Name;
};

class Module {
  std::vector<Function *> ContainedFs;
public:
  Module() {}
  ~Module() {
    for (auto i : ContainedFs)
      delete i;
    ContainedFs.clear();
  }
  Module *get() {
    return new Module;
  }
  void appendFunction(Function *F) {
    ContainedFs.push_back(F);
  }
  std::vector<Function *> getVal() {
    return ContainedFs;
  }
  void toLL(llvm::Module *M, SymbolTable *K);
};

class TypeVisitor
{
public:
  static llvm::Value *visit(AddInst *A);
};
}

#endif
