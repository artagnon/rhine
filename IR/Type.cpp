#include "rhine/IR/Type.h"
#include "rhine/Context.h"
#include <iostream>

namespace rhine {
Type::Type(RTType ID) : TyID(ID) {}

Type::~Type() {}

RTType Type::getTyID() const { return TyID; }

void Type::setSourceLocation(location SrcLoc) {
  SourceLoc = SrcLoc;
}

location Type::getSourceLocation() {
  return SourceLoc;
}

__attribute__((used, noinline))
void Type::dump() {
  std::cout << *this << std::endl;
}

UnType::UnType(): Type(RT_UnType) {}

UnType::~UnType() {}

UnType *UnType::get(Context *K) {
  auto Uniq = &K->UniqueUnType;
  Uniq->Kontext = K;
  return Uniq;
}

bool UnType::classof(const Type *T) {
  return T->getTyID() == RT_UnType;
}

void UnType::print(std::ostream &Stream) const {
  Stream << "UnType";
}

VoidType::VoidType(): Type(RT_VoidType) {}

VoidType::~VoidType() {}

VoidType *VoidType::get(Context *K) {
  auto Uniq = &K->UniqueVoidType;
  Uniq->Kontext = K;
  return Uniq;
}

bool VoidType::classof(const Type *T) {
  return T->getTyID() == RT_VoidType;
}

void VoidType::print(std::ostream &Stream) const {
  Stream << "()";
}

IntegerType::IntegerType(unsigned Width):
    Type(RT_IntegerType), Bitwidth(Width) {}

IntegerType::~IntegerType() {}

IntegerType *IntegerType::get(unsigned Bitwidth, Context *K) {
  FoldingSetNodeID ID;
  void *IP;
  IntegerType::Profile(ID, Bitwidth);
  if (auto T = K->ITyCache.FindNodeOrInsertPos(ID, IP))
    return T;
  auto T = new (K->RhAllocator) IntegerType(Bitwidth);
  K->ITyCache.InsertNode(T, IP);
  return T;
}

unsigned IntegerType::getBitwidth() {
  return Bitwidth;
}

bool IntegerType::classof(const Type *T) {
  return T->getTyID() == RT_IntegerType;
}

void IntegerType::Profile(FoldingSetNodeID &ID, const unsigned &W) {
  ID.AddInteger(W);
}

void IntegerType::Profile(FoldingSetNodeID &ID) {
  Profile(ID, Bitwidth);
}

void IntegerType::print(std::ostream &Stream) const {
  Stream << "Int";
}

BoolType::BoolType(): Type(RT_BoolType) {}

BoolType::~BoolType() {}

BoolType *BoolType::get(Context *K) {
  auto Uniq = &K->UniqueBoolType;
  Uniq->Kontext = K;
  return Uniq;
}

bool BoolType::classof(const Type *T) {
  return T->getTyID() == RT_BoolType;
}

void BoolType::print(std::ostream &Stream) const {
  Stream << "Bool";
}

FloatType::FloatType(): Type(RT_FloatType) {}

FloatType::~FloatType() {}

FloatType *FloatType::get(Context *K) {
  auto Uniq = &K->UniqueFloatType;
  Uniq->Kontext = K;
  return Uniq;
}

bool FloatType::classof(const Type *T) {
  return T->getTyID() == RT_FloatType;
}

void FloatType::print(std::ostream &Stream) const {
  Stream << "Float";
}

StringType::StringType(): Type(RT_StringType) {}

StringType::~StringType() {}

StringType *StringType::get(Context *K) {
  auto Uniq = &K->UniqueStringType;
  Uniq->Kontext = K;
  return Uniq;
}

bool StringType::classof(const Type *T) {
  return T->getTyID() == RT_StringType;
}

void StringType::print(std::ostream &Stream) const {
  Stream << "String";
}

FunctionType::FunctionType(Type *RTy, std::vector<Type *> ATys, bool IsV) :
    Type(RT_FunctionType), ReturnType(RTy), VariadicFlag(IsV),
    ArgumentTypes(ATys) {}

FunctionType::~FunctionType() {}

FunctionType *FunctionType::get(Type *RTy, std::vector<Type *> ATys,
                                bool IsV, Context *K) {
  FoldingSetNodeID ID;
  void *IP;
  FunctionType::Profile(ID, RTy, ATys, IsV);
  if (auto FTy = K->FTyCache.FindNodeOrInsertPos(ID, IP))
    return FTy;
  FunctionType *FTy = new (K->RhAllocator) FunctionType(RTy, ATys, IsV);
  K->FTyCache.InsertNode(FTy, IP);
  return FTy;
}

FunctionType *FunctionType::get(Type *RTy, Context *K) {
  return FunctionType::get(RTy, {}, false, K);
}

bool FunctionType::classof(const Type *T) {
  return T->getTyID() == RT_FunctionType;
}

void FunctionType::Profile(FoldingSetNodeID &ID, const Type *RTy,
                           const std::vector<Type *> ATys, bool IsV) {
  ID.AddPointer(RTy);
  for (auto &T: ATys)
    ID.AddPointer(T);
  ID.AddBoolean(IsV);
}

void FunctionType::Profile(FoldingSetNodeID &ID) {
  Profile(ID, ReturnType, ArgumentTypes, VariadicFlag);
}

Type *FunctionType::getATy(unsigned i) {
  assert(i < ArgumentTypes.size() && "getATy() out of bounds");
  return ArgumentTypes[i];
}

std::vector<Type *> FunctionType::getATys() {
  return ArgumentTypes;
}

Type *FunctionType::getRTy() {
  return ReturnType;
}

bool FunctionType::isVariadic() const {
  return VariadicFlag;
}

void FunctionType::print(std::ostream &Stream) const {
  Stream << "Fn(";
  if (!ArgumentTypes.size()) {
    if (isVariadic())
      Stream << "&";
    else
      Stream << "()";
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

PointerType::PointerType(Type *CTy) : Type(RT_PointerType), ContainedType(CTy) {}

PointerType::~PointerType() {}
PointerType *PointerType::get(Type *CTy, Context *K) {
  FoldingSetNodeID ID;
  void *IP;
  PointerType::Profile(ID, CTy);
  if (auto PTy = K->PTyCache.FindNodeOrInsertPos(ID, IP))
    return PTy;
  PointerType *PTy = new (K->RhAllocator) PointerType(CTy);
  K->PTyCache.InsertNode(PTy, IP);
  return PTy;
}

bool PointerType::classof(const Type *T) {
  return T->getTyID() == RT_PointerType;
}

void PointerType::Profile(FoldingSetNodeID &ID, const Type *CTy) {
  ID.AddPointer(CTy);
}

void PointerType::Profile(FoldingSetNodeID &ID) {
  Profile(ID, ContainedType);
}

Type *PointerType::getCTy() {
  return ContainedType;
}

void PointerType::print(std::ostream &Stream) const {
  Stream << *ContainedType << "*";
}
}
