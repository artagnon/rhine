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

void *ConstantInt::operator new(size_t s) {
  return User::operator new(s);
}

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

void *ConstantBool::operator new(size_t s) {
  return User::operator new(s);
}

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

void *ConstantFloat::operator new(size_t s) {
  return User::operator new(s);
}

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

Prototype::Prototype(std::string Name, FunctionType *FTy, RTValue RTy) :
    User(FTy, RTy, 0, Name), ParentModule(nullptr),
    VariadicRestLoadInst(nullptr) {}

Prototype::~Prototype() {}

void *Prototype::operator new(size_t s) {
  return User::operator new(s);
}

Prototype *Prototype::get(std::string Name, FunctionType *FTy) {
  return new Prototype(Name, FTy);
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

Function::Function(std::string Name, FunctionType *FTy) :
    Prototype(Name, FTy, RT_Function) {}

Function::~Function() {}

void *Function::operator new(size_t s) {
  return User::operator new(s);
}

Function *Function::get(std::string Name, FunctionType *FTy) {
  return new Function(Name, FTy);
}

bool Function::classof(const Value *V) {
  return V->getValID() == RT_Function;
}

std::string Function::getMangledName() const {
  return Name;
}

void Function::push_back(BasicBlock *NewBB) {
  Val.push_back(NewBB);
}

BasicBlock *Function::getEntryBlock() const {
  return Val.front();
}

BasicBlock *Function::getExitBlock() const {
  return Val.back();
}

Function::iterator Function::begin() {
  return Val.begin();
}

Function::iterator Function::end() {
  return Val.end();
}

void Function::print(std::ostream &Stream) const {
  Stream << "def " << Name;
  emitArguments(Stream);
  Stream << " ~" << *getType() << " {";
  for (auto V: *Val.front())
    Stream << std::endl << *V;
  Stream << std::endl << "}";
}

Pointer::Pointer(Value *V, Type *Ty) :
    User(Ty, RT_Pointer, 0, V->getName()), Val(V) {}

Pointer::~Pointer() {}

void *Pointer::operator new(size_t s) {
  return User::operator new(s);
}

Pointer *Pointer::get(Value *V) {
  auto Ty = PointerType::get(V->getType());
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