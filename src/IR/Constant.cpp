#include "rhine/IR/Context.h"
#include "rhine/IR/Constant.h"
#include "rhine/IR/Instruction.h"
#include "rhine/IR/UnresolvedValue.h"

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

bool ConstantInt::classof(const Value *V) {
  return V->getValID() == RT_ConstantInt;
}

int ConstantInt::getVal() const { return Val; }

unsigned ConstantInt::getBitwidth() const {
  if (auto ITy = dyn_cast<IntegerType>(VTy))
    return ITy->getBitwidth();
  assert(0 && "ConstantInt of non IntegerType type");
}

void ConstantInt::Profile(FoldingSetNodeID &ID, const Type *Ty,
                          const int &Val) {
  ID.Add(Ty);
  ID.AddInteger(Val);
}

void ConstantInt::Profile(FoldingSetNodeID &ID) const { Profile(ID, VTy, Val); }

void ConstantInt::print(std::ostream &Stream) const {
  Stream << Val << " ~" << *getType();
}

ConstantBool::ConstantBool(bool Val, Context *K)
    : Constant(BoolType::get(K), RT_ConstantBool), Val(Val) {}

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

void ConstantBool::print(std::ostream &Stream) const {
  Stream << Val << " ~" << *getType();
}

ConstantFloat::ConstantFloat(float Val, Context *K)
    : Constant(FloatType::get(K), RT_ConstantFloat), Val(Val) {}

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

void ConstantFloat::print(std::ostream &Stream) const {
  Stream << Val << " ~" << *getType();
}

Prototype::Prototype(std::string Name, FunctionType *FTy, RTValue RTTy)
    : Constant(FTy, RTTy, 0, Name), ParentModule(nullptr),
      VariadicRestLoadInst(nullptr) {}

Prototype::~Prototype() {
  for (auto A : ArgumentList)
    delete A;
  delete VariadicRestLoadInst;
}

void *Prototype::operator new(size_t s) { return User::operator new(s); }

Prototype *Prototype::get(std::string Name, FunctionType *FTy) {
  return new Prototype(Name, FTy);
}

bool Prototype::classof(const Value *V) {
  return V->getValID() >= RT_Prototype && V->getValID() <= RT_Function;
}

void Prototype::setParent(Module *Parent) { ParentModule = Parent; }

Module *Prototype::getParent() const { return ParentModule; }

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

void Prototype::setArguments(std::vector<Argument *> &L) {
  for (auto Arg : L)
    Arg->setParent(this);
  ArgumentList = L;
}

void Prototype::setVariadicRest(Argument *Rest) {
  if (!Rest)
    return;
  assert(cast<FunctionType>(VTy)->isVariadic() &&
         "Confusion about whether function is variadic");
  VariadicRestLoadInst = Rest;
}

std::vector<Argument *> Prototype::getArguments() const { return ArgumentList; }

Prototype::arg_iterator Prototype::arg_begin() { return ArgumentList.begin(); }

Prototype::arg_iterator Prototype::arg_end() { return ArgumentList.end(); }

iterator_range<Prototype::arg_iterator> Prototype::args() {
  return iterator_range<Prototype::arg_iterator>(arg_begin(), arg_end());
}

void Prototype::emitArguments(std::ostream &Stream) const {
  Stream << " [";
  if (ArgumentList.size()) {
    auto Terminator = *ArgumentList.rbegin();
    for (auto A : ArgumentList) {
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

Function::Function(std::string Name, FunctionType *FTy)
    : Prototype(Name, FTy, RT_Function) {}

Function::~Function() {
  for (auto *BB : Val)
    delete BB;
}

void *Function::operator new(size_t s) { return User::operator new(s); }

Function *Function::get(std::string Name, FunctionType *FTy) {
  return new Function(Name, FTy);
}

bool Function::classof(const Value *V) { return V->getValID() == RT_Function; }

std::string Function::getMangledName() const { return Name; }

void Function::push_back(BasicBlock *NewBB) { Val.push_back(NewBB); }

BasicBlock *Function::getEntryBlock() const { return Val.front(); }

BasicBlock *Function::getExitBlock() const { return Val.back(); }

Function::iterator Function::begin() { return Val.begin(); }

Function::iterator Function::end() { return Val.end(); }

void Function::print(std::ostream &Stream) const {
  Stream << "def " << Name;
  emitArguments(Stream);
  Stream << " ~" << *getType() << " {";
  for (auto V : *Val.front())
    Stream << std::endl << *V;
  Stream << std::endl << "}";
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

bool Pointer::classof(const Value *V) { return V->getValID() == RT_Pointer; }

void Pointer::setVal(Value *V) { Val = V; }

Value *Pointer::getVal() const { return Val; }

void Pointer::Profile(FoldingSetNodeID &ID, const Type *Ty, const Value *Val) {
  ID.Add(Ty);
  ID.AddPointer(Val);
}

void Pointer::Profile(FoldingSetNodeID &ID) const { Profile(ID, VTy, Val); }

void Pointer::print(std::ostream &Stream) const {
  Stream << *Val << "*" << std::endl;
}
}
