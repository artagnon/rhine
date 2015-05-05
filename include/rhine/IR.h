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
protected:
  /// Discriminator for LLVM-style RTTI (dyn_cast<> et al.)
  enum RTType {
      RT_UnType,
      RT_VoidType,
      RT_IntegerType,
      RT_BoolType,
      RT_FloatType,
      RT_StringType,
      RT_FunctionType,
  };
public:
  Type(RTType ID) : TyID(ID) {}
  RTType getTyID() const { return TyID; }
  static Type *get() = delete;
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
  virtual llvm::Type *toLL(llvm::Module *M = nullptr, Context *K = nullptr) {
    assert(false && "Cannot toLL() without inferring type");
  }
protected:
  virtual void print(std::ostream &Stream) const {
    Stream << "UnType";
  }
};

class IntegerType : public Type {
public:
  IntegerType(): Type(RT_IntegerType) {}
  static IntegerType *get(Context *K) {
    static auto UniqueIntegerType = new IntegerType();
    return UniqueIntegerType;
  }
  static bool classof(const Type *T) {
    return T->getTyID() == RT_IntegerType;
  }
  friend ostream &operator<<(ostream &Stream, const IntegerType &T) {
    T.print(Stream);
    return Stream;
  }
  llvm::Type *toLL(llvm::Module *M = nullptr, Context *K = nullptr);
protected:
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
    if (FunctionType *FTy = K->FTyCache.FindNodeOrInsertPos(ID, IP))
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
      Stream << "()";
    if (ReturnType)
      Stream << " -> " << *ReturnType << ")";
    else
      Stream << " -> ())";
  }
};

class Value : public FoldingSetNode {
protected:
  /// nullptr indicates VoidType
  Type *VTy;

  /// Discriminator for LLVM-style RTTI (dyn_cast<> et al.)
  enum RTType {
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
public:
  Value(Type *VTy, RTType ID) : VTy(VTy), ValID(ID) {}
  virtual ~Value() { };
  Value *get() = delete;
  RTType getValID() const { return ValID; }
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
  const RTType ValID;
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

class Constant : public Value {
public:
  Constant(Type *Ty, RTType ID) : Value(Ty, ID) {}
  friend ostream &operator<<(ostream &Stream, const Constant &C) {
    C.print(Stream);
    return Stream;
  }
  Type *typeInfer(Context *K = nullptr) = 0;
  llvm::Constant *toLL(llvm::Module *M = nullptr, Context *K = nullptr) = 0;
protected:
  virtual void print(std::ostream &Stream) const = 0;
};

class ConstantInt : public Constant {
  int Val;
public:
  ConstantInt(int Val, Context *K) :
      Constant(IntegerType::get(K), RT_ConstantInt), Val(Val) {}
  static ConstantInt *get(int Val, Context *K) {
    return new (K->RhAllocator) ConstantInt(Val, K);
  }
  static bool classof(const Value *V) {
    return V->getValID() == RT_ConstantInt;
  }
  int getVal() {
    return Val;
  }
  friend ostream &operator<<(ostream &Stream, const ConstantInt &I) {
    I.print(Stream);
    return Stream;
  }
  Type *typeInfer(Context *K = nullptr);
  llvm::Constant *toLL(llvm::Module *M = nullptr, Context *K = nullptr);
protected:
  virtual void print(std::ostream &Stream) const {
    Stream << Val << " ~" << *getType();
  }
};

class ConstantBool : public Constant {
  bool Val;
public:
  ConstantBool(bool Val, Context *K) :
      Constant(BoolType::get(K), RT_ConstantBool), Val(Val) {}
  static ConstantBool *get(bool Val, Context *K) {
    return new (K->RhAllocator) ConstantBool(Val, K);
  }
  static bool classof(const Value *V) {
    return V->getValID() == RT_ConstantBool;
  }
  float getVal() {
    return Val;
  }
  friend ostream &operator<<(ostream &Stream, const ConstantBool &B) {
    B.print(Stream);
    return Stream;
  }
  Type *typeInfer(Context *K = nullptr);
  llvm::Constant *toLL(llvm::Module *M = nullptr, Context *K = nullptr);
protected:
  virtual void print(std::ostream &Stream) const {
    Stream << Val << " ~" << *getType();
  }
};

class ConstantFloat : public Constant {
public:
  float Val;
  ConstantFloat(float Val, Context *K) :
      Constant(FloatType::get(K), RT_ConstantFloat), Val(Val) {}
  static ConstantFloat *get(float Val, Context *K) {
    return new (K->RhAllocator) ConstantFloat(Val, K);
  }
  static bool classof(const Value *V) {
    return V->getValID() == RT_ConstantFloat;
  }
  float getVal() {
    return Val;
  }
  friend ostream &operator<<(ostream &Stream, const ConstantFloat &F) {
    F.print(Stream);
    return Stream;
  }
  Type *typeInfer(Context *K = nullptr);
  llvm::Constant *toLL(llvm::Module *M = nullptr, Context *K = nullptr);
protected:
  virtual void print(std::ostream &Stream) const {
    Stream << Val << " ~" << *getType();
  }
};

class Function : public Constant {
  std::vector<Symbol *> ArgumentList;
  std::string Name;
  std::vector<Value *> Val;
public:
  Function(FunctionType *FTy) :
      Constant(FTy, RT_Function) {}
  static Function *get(FunctionType *FTy, Context *K) {
    return new (K->RhAllocator) Function(FTy);
  }
  static bool classof(const Value *V) {
    return V->getValID() == RT_Function;
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
  friend ostream &operator<<(ostream &Stream, const Function &F) {
    F.print(Stream);
    return Stream;
  }
  Type *typeInfer(Context *K = nullptr);
  llvm::Constant *toLL(llvm::Module *M = nullptr, Context *K = nullptr);
protected:
  virtual void print(std::ostream &Stream) const {
    Stream << Name << " ~" << *getType() << std::endl;
    for (auto A: ArgumentList)
      Stream << *A << std::endl;
    for (auto V: Val)
      Stream << *V << std::endl;
  }
};

class Instruction : public Value {
protected:
  std::vector<Value *> OperandList;
public:
  Instruction(Type *Ty, RTType ID) :
      Value(Ty, ID) {}
  void addOperand(Value *V) {
    OperandList.push_back(V);
  }
  Value *getOperand(unsigned i) {
    return OperandList[i];
  }
  friend ostream &operator<<(ostream &Stream, const Instruction &I) {
    I.print(Stream);
    return Stream;
  }
  virtual Type *typeInfer(Context *K = nullptr) = 0;
  virtual llvm::Value *toLL(llvm::Module *M = nullptr, Context *K = nullptr) = 0;
protected:
  virtual void print(std::ostream &Stream) const = 0;
};

class AddInst : public Instruction {
public:
  AddInst(Type *Ty) : Instruction(Ty, RT_AddInst) {}
  static AddInst *get(Type *Ty, Context *K) {
    return new (K->RhAllocator) AddInst(Ty);
  }
  static bool classof(const Value *V) {
    return V->getValID() == RT_AddInst;
  }
  friend ostream &operator<<(ostream &Stream, const AddInst &A) {
    A.print(Stream);
    return Stream;
  }
  Type *typeInfer(Context *K = nullptr);
  llvm::Value *toLL(llvm::Module *M = nullptr, Context *K = nullptr);
protected:
  virtual void print(std::ostream &Stream) const {
    Stream << "+ ~" << *getType() << std::endl;
    for (auto O: OperandList)
      Stream << *O << std::endl;
  }
};

class CallInst : public Instruction {
  std::string Name;
public:
  // May be untyped
  CallInst(std::string FunctionName, Type *Ty) :
      Instruction(Ty, RT_CallInst), Name(FunctionName) {}
  static CallInst *get(std::string FunctionName, Type *Ty, Context *K) {
    return new (K->RhAllocator) CallInst(FunctionName, Ty);
  }
  static bool classof(const Value *V) {
    return V->getValID() == RT_CallInst;
  }
  std::string getName() {
    return Name;
  }
  friend ostream &operator<<(ostream &Stream, const CallInst &C) {
    C.print(Stream);
    return Stream;
  }
  Type *typeInfer(Context *K = nullptr);
  llvm::Value *toLL(llvm::Module *M = nullptr, Context *K = nullptr);
protected:
  virtual void print(std::ostream &Stream) const {
    Stream << Name << " ~" << *getType() << std::endl;
    for (auto O: OperandList)
      Stream << *O << std::endl;
  }
};

class BindInst : public Instruction {
  std::string Name;
  Value *Val;
public:
  // This instruction cannot be an rvalue, and is of type Void
  BindInst(std::string N, Value *V) : Instruction(nullptr, RT_BindInst),
                                      Name(N), Val(V) {}

  static BindInst *get(std::string N, Value *V, Context *K) {
    return new (K->RhAllocator) BindInst(N, V);
  }
  static bool classof(const Value *V) {
    return V->getValID() == RT_BindInst;
  }
  Value *getVal() {
    return Val;
  }
  std::string getName() {
    return Name;
  }
  friend ostream &operator<<(ostream &Stream, const BindInst &S) {
    S.print(Stream);
    return Stream;
  }
  // Infers to Void, lowers to nothing
  Type *typeInfer(Context *K = nullptr);
  llvm::Value *toLL(llvm::Module *M = nullptr, Context *K = nullptr);
protected:
  virtual void print(std::ostream &Stream) const {
    Stream << Name << " = " << *Val << std::endl
           << *Val << " ~" << Val->getType();
  }
};

class Module {
  std::vector<Function *> ContainedFs;
public:
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
  friend ostream &operator<<(ostream &Stream, const Module &M) {
    M.print(Stream);
    return Stream;
  }
  void typeInfer(Context *K = nullptr);
  void toLL(llvm::Module *M, Context *K);
protected:
  virtual void print(std::ostream &Stream) const {
    for (auto F: ContainedFs)
      Stream << *F << std::endl;
  }
};
}

#endif
