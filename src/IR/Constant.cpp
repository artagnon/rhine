#include "rhine/IR/Context.hpp"
#include "rhine/IR/Constant.hpp"
#include "rhine/IR/Instruction.hpp"
#include "rhine/IR/UnresolvedValue.hpp"

namespace rhine {
Constant::Constant(Type *Ty, RTValue ID, unsigned NumOps, std::string Name) :
  User(Ty, ID, NumOps, Name) {}

Constant::~Constant() {}

bool Constant::classof(const Value *V) {
  return V->getValID() >= RT_ConstantInt && V->getValID() <= RT_ConstantFloat;
}

ConstantInt::ConstantInt(int Val, unsigned Bitwidth, Context *K)
    : Constant(IntegerType::get(Bitwidth, K), RT_ConstantInt), Val(Val) {}

ConstantInt::~ConstantInt() {}

void *ConstantInt::operator new(size_t s) { return User::operator new(s); }

ConstantInt *ConstantInt::get(int Val, unsigned Bitwidth, Context *K) {
  FoldingSetNodeID ID;
  void *IP;
  ConstantInt::Profile(ID, IntegerType::get(Bitwidth, K), Val);
  if (auto CInt = K->CIntCache.FindNodeOrInsertPos(ID, IP))
    return CInt;
  auto CInt = new ConstantInt(Val, Bitwidth, K);
  K->CIntCache.InsertNode(CInt, IP);
  return CInt;
}

IntegerType *ConstantInt::getType() const {
  return cast<IntegerType>(Value::getType());
}

bool ConstantInt::classof(const Value *V) {
  return V->getValID() == RT_ConstantInt;
}

int ConstantInt::getVal() const { return Val; }

unsigned ConstantInt::getBitwidth() const {
  if (auto ITy = dyn_cast<IntegerType>(VTy))
    return ITy->getBitwidth();
  assert(0 && "ConstantInt of non IntegerType type");
  return 0;
}

void ConstantInt::Profile(FoldingSetNodeID &ID, const Type *Ty,
                          const int &Val) {
  ID.Add(Ty);
  ID.AddInteger(Val);
}

void ConstantInt::Profile(FoldingSetNodeID &ID) const { Profile(ID, VTy, Val); }

void ConstantInt::print(DiagnosticPrinter &Stream) const {
  Stream << Val << " ~" << *getType();
}

ConstantBool::ConstantBool(bool Val, Context *K)
    : Constant(BoolType::get(K), RT_ConstantBool), Val(Val) {}

ConstantBool::~ConstantBool() {}

void *ConstantBool::operator new(size_t s) { return User::operator new(s); }

ConstantBool *ConstantBool::get(bool Val, Context *K) {
  FoldingSetNodeID ID;
  void *IP;
  ConstantBool::Profile(ID, BoolType::get(K), Val);
  if (auto CBool = K->CBoolCache.FindNodeOrInsertPos(ID, IP))
    return CBool;
  auto CBool = new ConstantBool(Val, K);
  K->CBoolCache.InsertNode(CBool, IP);
  return CBool;
}

BoolType *ConstantBool::getType() const {
  return cast<BoolType>(Value::getType());
}

bool ConstantBool::classof(const Value *V) {
  return V->getValID() == RT_ConstantBool;
}

float ConstantBool::getVal() const { return Val; }

void ConstantBool::Profile(FoldingSetNodeID &ID, const Type *Ty,
                           const bool &Val) {
  ID.Add(Ty);
  ID.AddBoolean(Val);
}

void ConstantBool::Profile(FoldingSetNodeID &ID) const {
  Profile(ID, VTy, Val);
}

void ConstantBool::print(DiagnosticPrinter &Stream) const {
  Stream << Val << " ~" << *getType();
}

ConstantFloat::ConstantFloat(float Val, Context *K)
    : Constant(FloatType::get(K), RT_ConstantFloat), Val(Val) {}

ConstantFloat::~ConstantFloat() {}

void *ConstantFloat::operator new(size_t s) { return User::operator new(s); }

ConstantFloat *ConstantFloat::get(float Val, Context *K) {
  FoldingSetNodeID ID;
  void *IP;
  ConstantFloat::Profile(ID, FloatType::get(K), Val);
  if (auto CFlt = K->CFltCache.FindNodeOrInsertPos(ID, IP))
    return CFlt;
  auto CFlt = new ConstantFloat(Val, K);
  K->CFltCache.InsertNode(CFlt, IP);
  return CFlt;
}

FloatType *ConstantFloat::getType() const {
  return cast<FloatType>(Value::getType());
}

bool ConstantFloat::classof(const Value *V) {
  return V->getValID() == RT_ConstantFloat;
}

float ConstantFloat::getVal() const { return Val; }

void ConstantFloat::Profile(FoldingSetNodeID &ID, const Type *Ty,
                            const float &Val) {
  ID.Add(Ty);
  ID.AddInteger((long)Val);
}

void ConstantFloat::Profile(FoldingSetNodeID &ID) const {
  Profile(ID, VTy, Val);
}

void ConstantFloat::print(DiagnosticPrinter &Stream) const {
  Stream << Val << " ~" << *getType();
}

Pointer::Pointer(Value *V, Type *Ty)
    : Constant(Ty, RT_Pointer, 0, V->getName()), Val(V) {}

Pointer::~Pointer() {}

void *Pointer::operator new(size_t s) { return User::operator new(s); }

Pointer *Pointer::get(Value *V) {
  FoldingSetNodeID ID;
  void *IP;
  auto Ty = PointerType::get(V->getType());
  Pointer::Profile(ID, Ty, V);
  auto K = V->getContext();
  if (auto Ptr = K->PtrCache.FindNodeOrInsertPos(ID, IP))
    return Ptr;
  auto Ptr = new Pointer(V, Ty);
  K->PtrCache.InsertNode(Ptr, IP);
  return Ptr;
}

PointerType *Pointer::getType() const {
  return cast<PointerType>(Value::getType());
}

bool Pointer::classof(const Value *V) { return V->getValID() == RT_Pointer; }

void Pointer::setVal(Value *V) { Val = V; }

Value *Pointer::getVal() const { return Val; }

void Pointer::Profile(FoldingSetNodeID &ID, const Type *Ty, const Value *Val) {
  ID.Add(Ty);
  ID.AddPointer(Val);
}

void Pointer::Profile(FoldingSetNodeID &ID) const { Profile(ID, VTy, Val); }

void Pointer::print(DiagnosticPrinter &Stream) const {
  Stream << *Val << "*" << std::endl;
}
}
