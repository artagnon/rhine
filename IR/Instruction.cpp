#include "rhine/IR/Instruction.h"

namespace rhine {
Instruction::Instruction(Type *Ty, RTValue ID, std::string N) :
    Value(Ty, ID, N) {}

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

AddInst *AddInst::get(Context *K) {
  return new (K->RhAllocator) AddInst(UnType::get(K));
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
    Instruction(Ty, RT_CallInst), Name(FunctionName) {}

CallInst *CallInst::get(std::string FunctionName, Context *K) {
  return new (K->RhAllocator) CallInst(FunctionName, UnType::get(K));
}

bool CallInst::classof(const Value *V) {
  return V->getValID() == RT_CallInst;
}

std::string CallInst::getName() {
  return Name;
}

void CallInst::print(std::ostream &Stream) const {
  Stream << Name << " ~" << *getType();
  for (auto O: OperandList)
    Stream << std::endl << *O;
}

MallocInst::MallocInst(std::string N, Type *Ty, Value *V) :
    Instruction(Ty, RT_MallocInst, N), Val(V) {}

MallocInst *MallocInst::get(std::string N, Value *V, Context *K) {
  return new (K->RhAllocator) MallocInst(
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

LoadInst *LoadInst::get(std::string N, Type *T, Context *K) {
  return new (K->RhAllocator) LoadInst(N, T);
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

IfInst *IfInst::get(Value * Conditional, BasicBlock *TrueBB,
                    BasicBlock *FalseBB, Context *K) {
  return new (K->RhAllocator) IfInst(UnType::get(K), Conditional,
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
