#include "rhine/IR/Function.hpp"
#include "rhine/IR/Context.hpp"
#include "rhine/IR/Instruction.hpp"
#include "rhine/IR/UnresolvedValue.hpp"

namespace rhine {
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

void Prototype::emitArguments(DiagnosticPrinter &Stream) const {
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

void Prototype::print(DiagnosticPrinter &Stream) const {
  Stream << "def " << Name;
  emitArguments(Stream);
  Stream << *getType();
}

Function::Function(std::string Name, FunctionType *FTy)
    : Prototype(Name, FTy, RT_Function) {}

Function::~Function() {
  dropAllReferences();
  for (auto &BB : Body)
    delete BB;
  Body.clear();
}

void Function::dropAllReferences() {
  for (auto &BB : *this)
    BB->dropAllReferences();
}

void *Function::operator new(size_t s) { return User::operator new(s); }

Function *Function::get(std::string Name, FunctionType *FTy) {
  return new Function(Name, FTy);
}

bool Function::classof(const Value *V) { return V->getValID() == RT_Function; }

std::string Function::getMangledName() const { return Name; }

void Function::push_back(BasicBlock *NewBB) {
  NewBB->setParent(this);
  Body.push_back(NewBB);
}

BasicBlock *Function::getEntryBlock() const { return Body.front(); }

BasicBlock *Function::getExitBlock() const { return Body.back(); }

Function::iterator Function::begin() { return Body.begin(); }

Function::iterator Function::end() { return Body.end(); }

void Function::print(DiagnosticPrinter &Stream) const {
  Stream << "def " << Name;
  emitArguments(Stream);
  Stream << " " << *getType() << " {";
  for (auto &BB : Body)
    Stream << std::endl << *BB;
  Stream << "}";
}
}
