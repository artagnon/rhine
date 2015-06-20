#include "rhine/IR/Type.h"
#include <iostream>

namespace rhine {
__attribute__((used, noinline))
void Type::dump() {
  std::cout << *this << std::endl;
}

RTType Type::getTyID() const { return TyID; }

void Type::setSourceLocation(location SrcLoc) {
  SourceLoc = SrcLoc;
}

location Type::getSourceLocation() {
  return SourceLoc;
}

FunctionType::FunctionType(Type *RTy, std::vector<Type *> ATys, bool IsV) :
    Type(RT_FunctionType), ReturnType(RTy), IsVariadic(IsV),
    ArgumentTypes(ATys) {}

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
  Profile(ID, ReturnType, ArgumentTypes, IsVariadic);
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

bool FunctionType::isVariadic() {
  return IsVariadic;
}

void FunctionType::print(std::ostream &Stream) const {
  Stream << "Fn(";
  if (!ArgumentTypes.size())
    Stream << "()";
  else {
    Stream << *ArgumentTypes[0];
    for (auto ATy = std::next(std::begin(ArgumentTypes));
         ATy != std::end(ArgumentTypes); ++ATy)
      Stream << " -> " << **ATy;
  }
  Stream << " -> " << *ReturnType << ")";
}
}
