#include "rhine/IR/Instruction.h"

namespace rhine {
Instruction::Instruction(Type *Ty, RTValue ID) : Value(Ty, ID) {}

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
  Stream << "+ ~" << *getType() << std::endl;
  for (auto O: OperandList)
    Stream << *O << std::endl;
}

CallInst::CallInst(std::string FunctionName, Type *Ty) :
    Instruction(Ty, RT_CallInst), Name(FunctionName),
    CallName(FunctionName) {}

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
  Stream << Name << " ~" << *getType() << std::endl;
  for (auto O: OperandList)
    Stream << *O << std::endl;
}

BindInst::BindInst(std::string N, Type *Ty, Value *V) :
    Instruction(Ty, RT_BindInst), Name(N), Val(V) {}

BindInst *BindInst::get(std::string N, Value *V, Context *K) {
  return new (K->RhAllocator) BindInst(N, VoidType::get(K), V);
}

bool BindInst::classof(const Value *V) {
  return V->getValID() == RT_BindInst;
}

void BindInst::setVal(Value *V) {
  Val = V;
}

Value *BindInst::getVal() {
  return Val;
}

std::string BindInst::getName() {
  return Name;
}

void BindInst::print(std::ostream &Stream) const {
  Stream << Name << " = " << *Val;
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
  Stream << "}" << std::endl;
}
}
