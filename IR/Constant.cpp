#include "rhine/IR/Constant.h"
#include "rhine/IR/UnresolvedValue.h"

namespace rhine {
Constant::Constant(Type *Ty, RTValue ID) : User(Ty, ID) {}

bool Constant::classof(const Value *V) {
  return V->getValID() >= RT_ConstantInt &&
    V->getValID() <= RT_ConstantFloat;
}

ConstantInt::ConstantInt(int Val, unsigned Bitwidth, Context *K) :
    Constant(IntegerType::get(Bitwidth, K), RT_ConstantInt), Val(Val) {}

ConstantInt *ConstantInt::get(int Val, unsigned Bitwidth, Context *K) {
  return new ConstantInt(Val, Bitwidth, K);
}

bool ConstantInt::classof(const Value *V) {
  return V->getValID() == RT_ConstantInt;
}

int ConstantInt::getVal() const {
  return Val;
}

unsigned ConstantInt::getBitwidth() const {
  if (auto ITy = dyn_cast<IntegerType>(VTy))
    return ITy->getBitwidth();
  assert(0 && "ConstantInt of non IntegerType type");
}

void ConstantInt::print(std::ostream &Stream) const {
  Stream << Val << " ~" << *getType();
}

ConstantBool::ConstantBool(bool Val, Context *K) :
    Constant(BoolType::get(K), RT_ConstantBool), Val(Val) {}

ConstantBool *ConstantBool::get(bool Val, Context *K) {
  return new ConstantBool(Val, K);
}

bool ConstantBool::classof(const Value *V) {
  return V->getValID() == RT_ConstantBool;
}

float ConstantBool::getVal() const {
  return Val;
}

void ConstantBool::print(std::ostream &Stream) const {
  Stream << Val << " ~" << *getType();
}

ConstantFloat::ConstantFloat(float Val, Context *K) :
    Constant(FloatType::get(K), RT_ConstantFloat), Val(Val) {}

ConstantFloat *ConstantFloat::get(float Val, Context *K) {
  return new ConstantFloat(Val, K);
}

bool ConstantFloat::classof(const Value *V) {
  return V->getValID() == RT_ConstantFloat;
}

float ConstantFloat::getVal() const {
  return Val;
}

void ConstantFloat::print(std::ostream &Stream) const {
  Stream << Val << " ~" << *getType();
}

Prototype::Prototype(FunctionType *FTy, RTValue RTy) :
    User(FTy, RTy), ParentModule(nullptr),
    VariadicRestLoadInst(nullptr) {}

Prototype::~Prototype() {}

Prototype *Prototype::get(FunctionType *FTy) {
  return new Prototype(FTy);
}

bool Prototype::classof(const Value *V) {
  return V->getValID() >= RT_Prototype &&
    V->getValID() <= RT_Function;
}

void Prototype::setParent(Module *Parent) {
  ParentModule = Parent;
}

Module *Prototype::getParent() const {
  return ParentModule;
}

void Prototype::setName(std::string N) {
  Name = N;
}

std::string Prototype::getName() const {
  return Name;
}

std::string Prototype::getMangledName() const {
  std::ostringstream Scratch;
  auto FTy = cast<FunctionType>(VTy);
  Scratch << "std_" << *FTy->getRTy() << "_" << Name << "__";
  auto ATys = FTy->getATys();
  Scratch << *ATys.front();
  ATys.erase(ATys.begin());
  for (auto ATy : ATys)
    Scratch << "_" << *ATy;
  return Scratch.str();
}

void Prototype::setArguments(std::vector<Argument *> L) {
  ArgumentList = L;
}

void Prototype::setVariadicRest(Argument *Rest) {
  if (!Rest)
    return;
  assert(cast<FunctionType>(VTy)->isVariadic() &&
         "Confusion about whether function is variadic");
  VariadicRestLoadInst = Rest;
}

std::vector<Argument *> Prototype::getArguments() const {
  return ArgumentList;
}

Prototype::arg_iterator Prototype::arg_begin() {
  return ArgumentList.begin();
}

Prototype::arg_iterator Prototype::arg_end() {
  return ArgumentList.end();
}

iterator_range<Prototype::arg_iterator> Prototype::args() {
  return iterator_range<Prototype::arg_iterator>(arg_begin(), arg_end());
}

void Prototype::emitArguments(std::ostream &Stream) const {
  Stream << " [";
  if (ArgumentList.size()) {
    auto Terminator = *ArgumentList.rbegin();
    for (auto A: ArgumentList) {
      Stream << *A;
      if (A != Terminator)
        Stream << " ";
    }
  }
  if (VariadicRestLoadInst) {
    if (ArgumentList.size())
      Stream << " ";
    Stream << "&" << *VariadicRestLoadInst;
  }
  Stream << "]";
}

void Prototype::print(std::ostream &Stream) const {
  Stream << "def " << Name;
  emitArguments(Stream);
  Stream << " ~" << *getType();
}

Function::Function(FunctionType *FTy) :
    Prototype(FTy, RT_Function), Val(nullptr) {}

Function::~Function() {}

Function *Function::get(FunctionType *FTy) {
  return new Function(FTy);
}

bool Function::classof(const Value *V) {
  return V->getValID() == RT_Function;
}

void Function::setBody(BasicBlock *Body) {
  Val = Body;
}

BasicBlock *Function::getVal() const {
  return Val;
}

BasicBlock *Function::getEntryBlock() const {
  return Val;
}

BasicBlock::iterator Function::begin() {
  return Val->begin();
}

BasicBlock::iterator Function::end() {
  return Val->end();
}

void Function::print(std::ostream &Stream) const {
  Stream << "def " << Name;
  emitArguments(Stream);
  Stream << " ~" << *getType() << " {";
  for (auto V: Val->ValueList)
    Stream << std::endl << *V;
  Stream << std::endl << "}";
}

Pointer::Pointer(Value *V, Type *Ty) :
    User(Ty, RT_Pointer, 0, V->getName()), Val(V) {}

Pointer::~Pointer() {}

Pointer *Pointer::get(Value *V) {
  auto K = V->getContext();
  auto Ty = PointerType::get(V->getType(), K);
  return new Pointer(V, Ty);
}

bool Pointer::classof(const Value *V) {
  return V->getValID() == RT_Pointer;
}

void Pointer::setVal(Value *V) {
  Val = V;
}

Value *Pointer::getVal() const {
  return Val;
}

void Pointer::print(std::ostream &Stream) const {
  Stream << *Val << "*" << std::endl;
}
}
