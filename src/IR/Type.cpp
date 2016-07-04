#include "rhine/IR/Type.hpp"
#include "rhine/IR/Context.hpp"
#include <iostream>

using Location = rhine::Parser::Location;

namespace rhine {
Type::Type(RTType ID) : TyID(ID) {}

Type::Type(Context *K, RTType ID) : Kontext(K), TyID(ID) {}

Context *Type::context() { return Kontext; }

RTType Type::getTyID() const { return TyID; }

void Type::setSourceLocation(Location SrcLoc) { SourceLoc = SrcLoc; }

Location Type::sourceLocation() { return SourceLoc; }

__attribute__((used, noinline)) void Type::dump() {
  std::cout << *this << std::endl;
}

UnType::UnType(Context *K) : Type(K, RT_UnType) {}

UnType *UnType::get(Context *K) {
  auto N = new UnType(K);
  K->UTyCache.push_back(N);
  return N;
}

bool UnType::classof(const Type *T) { return T->getTyID() == RT_UnType; }

void UnType::print(DiagnosticPrinter &Stream) const { Stream << "UnType"; }

VoidType::VoidType() : Type(RT_VoidType) {}

VoidType *VoidType::get(Context *K) {
  auto Uniq = &K->UniqueVoidType;
  Uniq->Kontext = K;
  return Uniq;
}

bool VoidType::classof(const Type *T) { return T->getTyID() == RT_VoidType; }

void VoidType::print(DiagnosticPrinter &Stream) const { Stream << "Void"; }

IntegerType::IntegerType(Context *K, unsigned Width)
    : Type(K, RT_IntegerType), Bitwidth(Width) {}

IntegerType *IntegerType::get(unsigned Bitwidth, Context *K) {
  FoldingSetNodeID ID;
  void *IP;
  IntegerType::Profile(ID, Bitwidth);
  if (auto T = K->ITyCache.FindNodeOrInsertPos(ID, IP))
    return T;
  auto T = new IntegerType(K, Bitwidth);
  K->ITyCache.InsertNode(T, IP);
  return T;
}

unsigned IntegerType::getBitwidth() { return Bitwidth; }

bool IntegerType::classof(const Type *T) {
  return T->getTyID() == RT_IntegerType;
}

void IntegerType::Profile(FoldingSetNodeID &ID, const unsigned &W) {
  ID.AddInteger(W);
}

void IntegerType::Profile(FoldingSetNodeID &ID) const { Profile(ID, Bitwidth); }

void IntegerType::print(DiagnosticPrinter &Stream) const { Stream << "Int"; }

BoolType::BoolType() : Type(RT_BoolType) {}

BoolType *BoolType::get(Context *K) {
  auto Uniq = &K->UniqueBoolType;
  Uniq->Kontext = K;
  return Uniq;
}

bool BoolType::classof(const Type *T) { return T->getTyID() == RT_BoolType; }

void BoolType::print(DiagnosticPrinter &Stream) const { Stream << "Bool"; }

FloatType::FloatType() : Type(RT_FloatType) {}

FloatType *FloatType::get(Context *K) {
  auto Uniq = &K->UniqueFloatType;
  Uniq->Kontext = K;
  return Uniq;
}

bool FloatType::classof(const Type *T) { return T->getTyID() == RT_FloatType; }

void FloatType::print(DiagnosticPrinter &Stream) const { Stream << "Float"; }

StringType::StringType() : Type(RT_StringType) {}

StringType *StringType::get(Context *K) {
  auto Uniq = &K->UniqueStringType;
  Uniq->Kontext = K;
  return Uniq;
}

bool StringType::classof(const Type *T) {
  return T->getTyID() == RT_StringType;
}

void StringType::print(DiagnosticPrinter &Stream) const { Stream << "String"; }

FunctionType::FunctionType(Context *K, Type *RTy, std::vector<Type *> ATys,
                           bool IsV)
    : Type(K, RT_FunctionType), ReturnType(RTy), VariadicFlag(IsV),
      ArgumentTypes(ATys) {}

FunctionType *FunctionType::get(Type *RTy, std::vector<Type *> ATys, bool IsV) {
  FoldingSetNodeID ID;
  void *IP;
  FunctionType::Profile(ID, RTy, ATys, IsV);
  auto K = RTy->context();
  if (auto FTy = K->FTyCache.FindNodeOrInsertPos(ID, IP))
    return FTy;
  FunctionType *FTy = new FunctionType(K, RTy, ATys, IsV);
  K->FTyCache.InsertNode(FTy, IP);
  return FTy;
}

FunctionType *FunctionType::get(Type *RTy) {
  return FunctionType::get(RTy, {}, false);
}

bool FunctionType::classof(const Type *T) {
  return T->getTyID() == RT_FunctionType;
}

void FunctionType::Profile(FoldingSetNodeID &ID, const Type *RTy,
                           const std::vector<Type *> &ATys, const bool &IsV) {
  ID.Add(RTy);
  for (auto &T : ATys)
    ID.Add(T);
  ID.AddBoolean(IsV);
}

void FunctionType::Profile(FoldingSetNodeID &ID) const {
  Profile(ID, ReturnType, ArgumentTypes, VariadicFlag);
}

Type *FunctionType::getATy(unsigned i) {
  assert(i < ArgumentTypes.size() && "getATy() out of bounds");
  return ArgumentTypes[i];
}

std::vector<Type *> FunctionType::getATys() { return ArgumentTypes; }

Type *FunctionType::returnType() { return ReturnType; }

bool FunctionType::isVariadic() const { return VariadicFlag; }

void FunctionType::print(DiagnosticPrinter &Stream) const {
  Stream << "Fn(";
  if (!ArgumentTypes.size()) {
    if (isVariadic())
      Stream << "&";
    else
      Stream << "Void";
  } else {
    Stream << *ArgumentTypes[0];
    for (auto ATy = std::next(std::begin(ArgumentTypes));
         ATy != std::end(ArgumentTypes); ++ATy)
      Stream << " -> " << **ATy;
    if (isVariadic())
      Stream << " -> &";
  }
  Stream << " -> " << *ReturnType << ")";
}

PointerType::PointerType(RTType RTy, Type *CTy)
    : Type(CTy->context(), RTy), ContainedType(CTy) {}

PointerType *PointerType::get(Type *CTy) {
  FoldingSetNodeID ID;
  void *IP;
  PointerType::Profile(ID, CTy);
  auto K = CTy->context();
  if (auto PTy = K->PTyCache.FindNodeOrInsertPos(ID, IP))
    return PTy;
  PointerType *PTy = new PointerType(RT_PointerType, CTy);
  K->PTyCache.InsertNode(PTy, IP);
  return PTy;
}

bool PointerType::classof(const Type *T) {
  return T->getTyID() == RT_PointerType || T->getTyID() == RT_TensorType;
}

void PointerType::Profile(FoldingSetNodeID &ID, const Type *CTy) {
  ID.Add(CTy);
}

void PointerType::Profile(FoldingSetNodeID &ID) const {
  Profile(ID, ContainedType);
}

Type *PointerType::containedType() { return ContainedType; }

size_t PointerType::getSize() { return 8; }

void PointerType::print(DiagnosticPrinter &Stream) const {
  Stream << *ContainedType << "*";
}

TensorType::TensorType(Type *CTy, std::vector<size_t> &Dims)
    : PointerType(RT_TensorType, CTy), Dimensions(Dims) {}

TensorType *TensorType::get(Type *CTy, std::vector<size_t> &Dims) {
  FoldingSetNodeID ID;
  void *IP;
  TensorType::Profile(ID, CTy, Dims);
  auto K = CTy->context();
  if (auto TTy = K->TTyCache.FindNodeOrInsertPos(ID, IP))
    return TTy;
  auto TTy = new TensorType(CTy, Dims);
  K->PTyCache.InsertNode(TTy, IP);
  return TTy;
}

bool TensorType::classof(const Type *T) {
  return T->getTyID() == RT_TensorType;
}

void TensorType::Profile(FoldingSetNodeID &ID, const Type *CTy,
                         const std::vector<size_t> &Dims) {
  for (auto Dim : Dims) {
    ID.AddInteger(Dim);
  }
  ID.Add(CTy);
}

void TensorType::Profile(FoldingSetNodeID &ID) const {
  Profile(ID, ContainedType, Dimensions);
}

std::vector<size_t> TensorType::getDims() { return Dimensions; }

void TensorType::print(DiagnosticPrinter &Stream) const {
  Stream << "[" << Dimensions[0];
  for (auto Dim = std::next(Dimensions.begin()); Dim != Dimensions.end();
       Dim = std::next(Dim)) {
    Stream << "x" << *Dim;
  }
  Stream << "]" << *ContainedType;
}
}
