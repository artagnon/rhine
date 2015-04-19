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
  static Type *get() = delete;
  friend ostream &operator<<(ostream &Stream, const Type &T) {
    T.print(Stream);
    return Stream;
  }
  virtual llvm::Type *toLL(llvm::Module *M = nullptr, Context *K = nullptr) = 0;
protected:
  virtual void print(std::ostream &Stream) const = 0;
};

class UnType : public Type {
public:
  static UnType *get() {
    static auto UniqueUnType = new UnType();
    return UniqueUnType;
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
  static IntegerType *get() {
    static auto UniqueIntegerType = new IntegerType();
    return UniqueIntegerType;
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
  static BoolType *get() {
    static auto UniqueBoolType = new BoolType();
    return UniqueBoolType;
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
  static FloatType *get() {
    static auto UniqueFloatType = new FloatType();
    return UniqueFloatType;
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
  static StringType *get() {
    static auto UniqueStringType = new StringType();
    return UniqueStringType;
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
  Type *getATy(unsigned i) {
    return ArgumentTypes[i];
  }
  Type *getRTy() {
    return ReturnType;
  }
  void setRTy(Type *T) {
    ReturnType = T;
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
      Stream << ArgumentTypes[0];
      for (auto ATy = std::next(std::begin(ArgumentTypes));
           ATy != std::end(ArgumentTypes); ++ATy)
        Stream << ", " << *ATy;
    } else
      Stream << "()";
    Stream << " -> " << *ReturnType << ")";
  }
};

class Value : public FoldingSetNode {
protected:
  Type *VTy;
public:
  Value(Type *VTy) : VTy(VTy) {}
  virtual ~Value() { };
  Value *get() = delete;
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
  GlobalString(std::string Val) : Value(StringType::get()), Val(Val) {}
  static GlobalString *get(std::string Val) {
    return new GlobalString(Val);
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
  Constant(Type *Ty) : Value(Ty) {}
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
  ConstantInt(int Val) : Constant(IntegerType::get()), Val(Val) {}
  static ConstantInt *get(int Val) {
    return new ConstantInt(Val);
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
  ConstantBool(bool Val) : Constant(BoolType::get()), Val(Val) {}
  static ConstantBool *get(bool Val) {
    return new ConstantBool(Val);
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
  ConstantFloat(float Val) : Constant(FloatType::get()), Val(Val) {}
  static ConstantFloat *get(float Val) {
    return new ConstantFloat(Val);
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
  Instruction(Type *Ty) :
      Value(Ty) {}
  ~Instruction() {
    OperandList.clear();
  }
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
  AddInst(Type *Ty) : Instruction(Ty) {}
  static AddInst *get(Type *Ty) {
    return new AddInst(Ty);
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
  CallInst(std::string FunctionName, Type *Ty = IntegerType::get()) :
      Instruction(Ty), Name(FunctionName) {}
  static CallInst *get(std::string FunctionName, Type *Ty = IntegerType::get()) {
    return new CallInst(FunctionName, Ty);
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
