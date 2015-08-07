#include "rhine/IR/Instruction.h"

namespace rhine {
Instruction::Instruction(Type *Ty, RTValue ID, std::string N) :
    Value(Ty, ID, N) {}

bool Instruction::classof(const Value *V) {
  return V->getValID() >= RT_AddInst &&
    V->getValID() <= RT_IfInst;
}

void Instruction::addOperand(Value *V) {
  OperandList.push_back(V);
}

Value *Instruction::getOperand(unsigned i) {
  assert(i < OperandList.size() && "getOperand() out of range");
  return OperandList[i];
}

std::vector<Value *> Instruction::getOperands() {
  return OperandList;
}

void Instruction::setOperands(std::vector<Value *> Ops) {
  OperandList = Ops;
}

AddInst::AddInst(Type *Ty) : Instruction(Ty, RT_AddInst) {}

void *AddInst::operator new(size_t s) {
  return User::operator new(s, 2);
}

AddInst *AddInst::get(Context *K) {
  return new AddInst(UnType::get(K));
}

bool AddInst::classof(const Value *V) {
  return V->getValID() == RT_AddInst;
}

void AddInst::print(std::ostream &Stream) const {
  Stream << "+ ~" << *getType();
  for (auto O: OperandList)
    Stream << std::endl << *O;
}

CallInst::CallInst(std::string FunctionName, Type *Ty) :
    Instruction(Ty, RT_CallInst), Callee(FunctionName) {}

void *CallInst::operator new(size_t s, unsigned n) {
  return User::operator new(s, n);
}

CallInst *CallInst::get(std::string FunctionName,
                        unsigned NumOperands, Context *K) {
  return new (NumOperands) CallInst(FunctionName, UnType::get(K));
}

bool CallInst::classof(const Value *V) {
  return V->getValID() == RT_CallInst;
}

std::string CallInst::getCallee() {
  return Callee;
}

void CallInst::print(std::ostream &Stream) const {
  Stream << Callee << " ~" << *getType();
  for (auto O: OperandList)
    Stream << std::endl << *O;
}

MallocInst::MallocInst(std::string N, Type *Ty, Value *V) :
    Instruction(Ty, RT_MallocInst, N), Val(V) {}

void *MallocInst::operator new(size_t s) {
  return User::operator new(s, 2);
}

MallocInst *MallocInst::get(std::string N, Value *V, Context *K) {
  return new MallocInst(
      N, VoidType::get(K), V);
}

bool MallocInst::classof(const Value *V) {
  return V->getValID() == RT_MallocInst;
}

void MallocInst::setVal(Value *V) {
  Val = V;
}

Value *MallocInst::getVal() {
  return Val;
}

void MallocInst::print(std::ostream &Stream) const {
  Stream << Name << " = " << *Val;
}

LoadInst::LoadInst(std::string N, Type *T, RTValue ID) :
    Instruction(T, ID, N) {}

void *LoadInst::operator new(size_t s) {
  return User::operator new(s, 1);
}

LoadInst *LoadInst::get(std::string N, Type *T, Context *K) {
  return new LoadInst(N, T);
}

bool LoadInst::classof(const Value *V) {
  return V->getValID() == RT_LoadInst;
}

void LoadInst::print(std::ostream &Stream) const {
  Stream << Name << " ~" << *getType();
}

IfInst::IfInst(Type *Ty, Value * Conditional_,
               BasicBlock *TrueBB_, BasicBlock *FalseBB_):
    Instruction(Ty, RT_IfInst), Conditional(Conditional_),
    TrueBB(TrueBB_), FalseBB(FalseBB_) {}

void *IfInst::operator new(size_t s) {
  return User::operator new(s, 2);
}

IfInst *IfInst::get(Value * Conditional, BasicBlock *TrueBB,
                    BasicBlock *FalseBB, Context *K) {
  return new IfInst(UnType::get(K), Conditional,
                    TrueBB, FalseBB);
}

bool IfInst::classof(const Value *V) {
  return V->getValID() == RT_IfInst;
}

Value *IfInst::getConditional() {
  return Conditional;
}

void IfInst::setConditional(Value *C)
{
  Conditional = C;
}

BasicBlock *IfInst::getTrueBB() {
  return TrueBB;
}
BasicBlock *IfInst::getFalseBB() {
  return FalseBB;
}

void IfInst::print(std::ostream &Stream) const {
  Stream << "if (" << *Conditional << ") {" << std::endl;
  for (auto V: *TrueBB)
    Stream << *V << std::endl;
  Stream << "} else {" << std::endl;
  for (auto V: *FalseBB)
    Stream << *V << std::endl;
  Stream << "}";
}
}
