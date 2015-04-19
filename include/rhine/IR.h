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
#include <sstream>

#include "rhine/Context.h"

using namespace std;

namespace rhine {
using namespace llvm;

static LLVMContext &RhContext = getGlobalContext();
static IRBuilder<> RhBuilder(RhContext);

class Type : public FoldingSetNode {
public:
  static Type *get() {
    static auto UniqueType = new Type();
    return UniqueType;
  }
  friend ostream &operator<<(ostream &Stream, const Type &T) {
    Stream << "~ Type";
    return Stream;
  }
  virtual ~Type() { };
  virtual llvm::Type *toLL(llvm::Module *M = nullptr, Context *K = nullptr) {
    assert(false && "Cannot toLL() without inferring type");
  }
};

class IntegerType : public Type {
public:
  static IntegerType *get() {
    static auto UniqueIntegerType = new IntegerType();
    return UniqueIntegerType;
  }
  friend ostream &operator<<(ostream &Stream, const IntegerType &T) {
    Stream << "~ IntegerType";
    return Stream;
  }
  llvm::Type *toLL(llvm::Module *M = nullptr, Context *K = nullptr);
};

class BoolType : public Type {
public:
  static BoolType *get() {
    static auto UniqueBoolType = new BoolType();
    return UniqueBoolType;
  }
  friend ostream &operator<<(ostream &Stream, const BoolType &T) {
    Stream << "~ BoolType";
    return Stream;
  }
  llvm::Type *toLL(llvm::Module *M = nullptr, Context *K = nullptr);
};

class FloatType : public Type {
public:
  static FloatType *get() {
    static auto UniqueFloatType = new FloatType();
    return UniqueFloatType;
  }
  friend ostream &operator<<(ostream &Stream, const FloatType &T) {
    Stream << "~ FloatType";
    return Stream;
  }
  llvm::Type *toLL(llvm::Module *M = nullptr, Context *K = nullptr);
};

class StringType : public Type {
public:
  static StringType *get() {
    static auto UniqueStringType = new StringType();
    return UniqueStringType;
  }
  friend ostream &operator<<(ostream &Stream, const StringType &T) {
    Stream << "~ StringType";
    return Stream;
  }
  llvm::Type *toLL(llvm::Module *M = nullptr, Context *K = nullptr);
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
  static FunctionType *get(R RTy, As... ATys, Context *K) {
    FoldingSetNodeID ID;
    void *InsertPos;
    FunctionType::Profile(ID, RTy, {ATys...});
    FunctionType *FTy = K->getFTyCache()->FindNodeOrInsertPos(ID, InsertPos);
    if (!FTy) {
      FTy = new FunctionType(RTy, ATys...);
      K->getFTyCache()->InsertNode(FTy, InsertPos);
    }
    return FTy;
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
  friend ostream &operator<<(ostream &Stream, const FunctionType &T) {
    Stream << "~ FunctionType";
    return Stream;
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
  llvm::Type *toLL(llvm::Module *M = nullptr, Context *K = nullptr);
};

template <typename T> class ArrayType : public Type {
public:
  T *elTy;
  llvm::Type *toLL(llvm::Module *M = nullptr, Context *K = nullptr);
};

class Value : public FoldingSetNode {
protected:
  Type *VTy;
public:
  Value(Type *VTy) : VTy(VTy) {}
  virtual ~Value() { };
  Value *get() = delete;
  Type *getType() {
    return VTy;
  }
  void setType(Type *T) {
    VTy = T;
  }
  virtual Type *typeInfer(Context *K = nullptr) = 0;
  virtual llvm::Value *toLL(llvm::Module *M = nullptr, Context *K = nullptr) = 0;
};

class Symbol : public Value {
  std::string Name;
public:
  Symbol(std::string N, Type *T) : Value(T), Name(N) {}

  static Symbol *get(std::string N, Type *T, Context *K) {
    FoldingSetNodeID ID;
    void *InsertPos;
    Symbol::Profile(ID, N, T);
    Symbol *S = K->getSymbolCache()->FindNodeOrInsertPos(ID, InsertPos);
    if (!S) {
      S = new Symbol(N, T);
      K->getSymbolCache()->InsertNode(S, InsertPos);
    }
    return S;
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
  Type *typeInfer(Context *K = nullptr);
  llvm::Value *toLL(llvm::Module *M = nullptr, Context *K = nullptr);
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
  Type *typeInfer(Context *K = nullptr);
  // Returns GEP to GlobalStringPtr, which is a Value; data itself is in
  // constant storage.
  llvm::Value *toLL(llvm::Module *M = nullptr, Context *K = nullptr);
};

class Constant : public Value {
public:
  Constant(Type *Ty) : Value(Ty) {}
  Type *typeInfer(Context *K = nullptr) = 0;
  llvm::Constant *toLL(llvm::Module *M = nullptr, Context *K = nullptr) = 0;
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
  Type *typeInfer(Context *K = nullptr);
  llvm::Constant *toLL(llvm::Module *M = nullptr, Context *K = nullptr);
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
  Type *typeInfer(Context *K = nullptr);
  llvm::Constant *toLL(llvm::Module *M = nullptr, Context *K = nullptr);
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
  Type *typeInfer(Context *K = nullptr);
  llvm::Constant *toLL(llvm::Module *M = nullptr, Context *K = nullptr);
};

class Function : public Constant {
  std::vector<Symbol *> ArgumentList;
  std::string Name;
  std::vector<Value *> Val;
public:
  Function(FunctionType *FTy) :
      Constant(FTy) {}
  ~Function() {
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
  Type *typeInfer(Context *K = nullptr);
  llvm::Constant *toLL(llvm::Module *M = nullptr, Context *K = nullptr);
};

class Instruction : public Value {
  unsigned NumOperands;
  std::vector<Value *> OperandList;
public:
  Instruction(Type *Ty) :
      Value(Ty) {}
  ~Instruction() {
    OperandList.clear();
  }
  void addOperand(Value *V) {
    OperandList.push_back(V);
    NumOperands++;
  }
  Value *getOperand(unsigned i) {
    return OperandList[i];
  }
  virtual Type *typeInfer(Context *K = nullptr) = 0;
  virtual llvm::Value *toLL(llvm::Module *M = nullptr, Context *K = nullptr) = 0;
};

class AddInst : public Instruction {
public:
  AddInst(Type *Ty) : Instruction(Ty) {}
  static AddInst *get(Type *Ty) {
    return new AddInst(Ty);
  }
  Type *typeInfer(Context *K = nullptr);
  llvm::Value *toLL(llvm::Module *M = nullptr, Context *K = nullptr);
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
  Type *typeInfer(Context *K = nullptr);
  llvm::Value *toLL(llvm::Module *M = nullptr, Context *K = nullptr);
private:
  std::string Name;
};

class Module {
  std::vector<Function *> ContainedFs;
public:
  Module() {}
  ~Module() {
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
  void setVal(std::vector<Function *> Fs) {
    ContainedFs = Fs;
  }
  void typeInfer(Context *K = nullptr);
  void toLL(llvm::Module *M, Context *K);
};
}

#endif
