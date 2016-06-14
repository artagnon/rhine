#include "rhine/IR/Instruction.hpp"
#include "rhine/IR/Constant.hpp"

namespace rhine {
Instruction::Instruction(Type *Ty, RTValue ID, unsigned NumOps, std::string N)
    : User(Ty, ID, NumOps, N) {}

bool Instruction::classof(const Value *V) {
  return V->op() >= RT_AddInst && V->op() <= RT_IndexingInst;
}

BasicBlock *Instruction::getParent() const { return Parent; }

void Instruction::setParent(BasicBlock *P) { Parent = P; }

BinaryArithInst::BinaryArithInst(RTValue InstSelector, Type *Ty, Value *Op0,
                                 Value *Op1)
    : Instruction(Ty, InstSelector, 2) {
  setOperand(0, Op0);
  setOperand(1, Op1);
}

void *BinaryArithInst::operator new(size_t S) {
  return User::operator new(S, 2);
}

BinaryArithInst *BinaryArithInst::get(RTValue InstSelector, Value *Op0,
                                      Value *Op1) {
  auto K = Op0->context();
  return new BinaryArithInst(InstSelector, UnType::get(K), Op0, Op1);
}

bool BinaryArithInst::classof(const Value *V) {
  return V->op() == RT_MulInst || V->op() == RT_DivInst ||
         V->op() == RT_AddInst || V->op() == RT_SubInst;
}

void BinaryArithInst::print(DiagnosticPrinter &Stream) const {
  switch (op()) {
  case RT_AddInst:
    Stream << "+ " << *VTy;
    break;
  case RT_SubInst:
    Stream << "- " << *VTy;
    break;
  case RT_MulInst:
    Stream << "* " << *VTy;
    break;
  case RT_DivInst:
    Stream << "/ " << *VTy;
    break;
  default:
    assert(0 && "Malformed BinaryArithInst; cannot print");
  }
  for (auto O : getOperands())
    Stream << std::endl << *O;
}

CallInst::CallInst(Type *Ty, unsigned NumOps, std::string N)
    : Instruction(Ty, RT_CallInst, NumOps, N) {}

void *CallInst::operator new(size_t S, unsigned N) {
  return User::operator new(S, N);
}

CallInst *CallInst::get(Value *Callee, std::vector<Value *> Ops) {
  auto NumOps = Ops.size();
  auto Name = Callee->getName();
  auto Obj = new (NumOps + 1) CallInst(Callee->getType(), NumOps, Name);
  Obj->NumAllocatedOps = NumOps + 1;
  Obj->setOperand(-1, Callee);
  for (unsigned OpN = 0; OpN < NumOps; OpN++)
    Obj->setOperand(OpN, Ops[OpN]);
  return Obj;
}

bool CallInst::classof(const Value *V) { return V->op() == RT_CallInst; }

FunctionType *CallInst::getFTy() const {
  auto PTy = cast<PointerType>(getType());
  assert(PTy && "Illegal call to getFTy() before type inference");
  return cast<FunctionType>(PTy->getCTy());
}

std::vector<Type *> CallInst::getATys() const { return getFTy()->getATys(); }

Type *CallInst::getRTy() const { return getFTy()->getRTy(); }

Value *CallInst::getCallee() const { return getOperand(-1); }

void CallInst::print(DiagnosticPrinter &Stream) const {
  Stream << getCallee()->getName() << " " << *VTy;
  for (auto O : getOperands())
    Stream << std::endl << *O;
}

AbstractBindInst::AbstractBindInst(RTValue RTVal, std::string N, Value *V)
    : Instruction(V->getType(), RTVal, 1, N) {
  setOperand(0, V);
  setSourceLocation(V->sourceLocation());
}

bool AbstractBindInst::classof(const Value *V) {
  return V->op() == RT_BindInst || V->op() == RT_MallocInst;
}

Value *AbstractBindInst::val() { return getOperand(0); }

void AbstractBindInst::setVal(Value *V) { setOperand(0, V); }

BindInst::BindInst(std::string N, Value *V)
    : AbstractBindInst(RT_BindInst, N, V) {}

void *BindInst::operator new(size_t S) { return User::operator new(S, 1); }

BindInst *BindInst::get(std::string N, Value *V) { return new BindInst(N, V); }

bool BindInst::classof(const Value *V) { return V->op() == RT_BindInst; }

Value *BindInst::val() { return AbstractBindInst::val(); }

void BindInst::setVal(Value *V) { AbstractBindInst::setVal(V); }

void BindInst::print(DiagnosticPrinter &Stream) const {
  Stream << Name << " = " << *getOperand(0);
}

MallocInst::MallocInst(std::string N, Value *V)
    : AbstractBindInst(RT_MallocInst, N, V) {}

void *MallocInst::operator new(size_t S) { return User::operator new(S, 1); }

MallocInst *MallocInst::get(std::string N, Value *V) {
  return new MallocInst(N, V);
}

bool MallocInst::classof(const Value *V) {
  return V->op() == RT_MallocInst;
}

Value *MallocInst::val() { return AbstractBindInst::val(); }

void MallocInst::setVal(Value *V) { AbstractBindInst::setVal(V); }

void MallocInst::print(DiagnosticPrinter &Stream) const {
  Stream << Name << " = malloc:" << *getOperand(0);
}

LoadInst::LoadInst(MallocInst *M)
    : Instruction(M->getType(), RT_LoadInst, 1, M->getName()) {
  setOperand(0, M);
}

LoadInst::~LoadInst() {}

void *LoadInst::operator new(size_t S) { return User::operator new(S, 1); }

LoadInst *LoadInst::get(MallocInst *M) { return new LoadInst(M); }

bool LoadInst::classof(const Value *V) { return V->op() == RT_LoadInst; }

Value *LoadInst::val() const { return getOperand(0); }

void LoadInst::print(DiagnosticPrinter &Stream) const {
  Stream << Name << " " << *VTy;
}

StoreInst::StoreInst(Value *MallocedValue, Value *NewValue)
    : Instruction(MallocedValue->getType(), RT_StoreInst, 2) {
  assert(MallocedValue->getType() == NewValue->getType() &&
         "Type mismatch in store");
  setOperand(0, MallocedValue);
  setOperand(1, NewValue);
}

StoreInst::~StoreInst() {}

void *StoreInst::operator new(size_t S) { return User::operator new(S, 2); }

StoreInst *StoreInst::get(Value *MallocedValue, Value *NewValue) {
  return new StoreInst(MallocedValue, NewValue);
}

bool StoreInst::classof(const Value *V) {
  return V->op() == RT_StoreInst;
}

Value *StoreInst::getMallocedValue() const { return getOperand(0); }

Value *StoreInst::getNewValue() const { return getOperand(1); }

void StoreInst::print(DiagnosticPrinter &Stream) const {
  Stream << *getMallocedValue() << " = " << *getNewValue();
}

ReturnInst::ReturnInst(Type *Ty, bool IsVoid)
    : Instruction(Ty, RT_ReturnInst, !IsVoid) {}

ReturnInst::~ReturnInst() {}

void *ReturnInst::operator new(size_t S, unsigned N) {
  return User::operator new(S, N);
}

ReturnInst *ReturnInst::get(Value *V, Context *K) {
  if (!V)
    return new (0) ReturnInst(VoidType::get(K), true);
  auto Obj = new (1) ReturnInst(V->getType(), false);
  Obj->setOperand(0, V);
  return Obj;
}

bool ReturnInst::classof(const Value *V) {
  return V->op() == RT_ReturnInst;
}

void ReturnInst::setVal(Value *V) { setOperand(0, V); }

Value *ReturnInst::val() {
  auto Operands = getOperands();
  if (Operands.size())
    return Operands[0];
  return nullptr;
}

void ReturnInst::print(DiagnosticPrinter &Stream) const {
  Stream << "ret " << *getOperand(0);
}

TerminatorInst::TerminatorInst(Type *Ty)
    : Instruction(Ty, RT_TerminatorInst, 1) {}

TerminatorInst::~TerminatorInst() {}

void *TerminatorInst::operator new(size_t S) {
  return User::operator new(S, 1);
}

TerminatorInst *TerminatorInst::get(Value *V) {
  auto Obj = new TerminatorInst(V->getType());
  Obj->setOperand(0, V);
  return Obj;
}

bool TerminatorInst::classof(const Value *V) {
  return V->op() == RT_TerminatorInst;
}

void TerminatorInst::setVal(Value *V) { setOperand(0, V); }

Value *TerminatorInst::val() { return getOperand(0); }

void TerminatorInst::print(DiagnosticPrinter &Stream) const {
  Stream << "term " << *getOperand(0);
}

IfInst::IfInst(Type *Ty) : Instruction(Ty, RT_IfInst, 3) {}

IfInst::~IfInst() {}

void *IfInst::operator new(size_t S) { return User::operator new(S, 3); }

IfInst *IfInst::get(Value *Conditional, BasicBlock *TrueBB,
                    BasicBlock *FalseBB) {
  auto Obj = new IfInst(UnType::get(Conditional->context()));
  TrueBB->setName("true");
  FalseBB->setName("false");
  Obj->setOperand(0, Conditional);
  Obj->setOperand(1, TrueBB);
  Obj->setOperand(2, FalseBB);
  return Obj;
}

bool IfInst::classof(const Value *V) { return V->op() == RT_IfInst; }

Value *IfInst::getConditional() const { return getOperand(0); }

BasicBlock *IfInst::getTrueBB() const {
  return cast<BasicBlock>(getOperand(1));
}
BasicBlock *IfInst::getFalseBB() const {
  return cast<BasicBlock>(getOperand(2));
}

void IfInst::print(DiagnosticPrinter &Stream) const {
  Stream << "if (" << *getConditional() << ") {" << std::endl;
  for (auto V : *getTrueBB())
    Stream << *V << std::endl;
  Stream << "} else {" << std::endl;
  for (auto V : *getFalseBB())
    Stream << *V << std::endl;
  Stream << "}";
}

IndexingInst::IndexingInst(Type *Ty, unsigned NumOps)
    : Instruction(Ty, RT_IndexingInst, NumOps) {}

IndexingInst::~IndexingInst() {}

void *IndexingInst::operator new(size_t S, unsigned N) {
  return User::operator new(S, N);
}

IndexingInst *IndexingInst::get(Value *V, std::vector<Value *> &Idxes) {
  auto NumOps = Idxes.size();
  auto Inst = new (NumOps + 1) IndexingInst(V->getType(), NumOps);
  Inst->NumAllocatedOps = NumOps + 1;
  Inst->setOperand(-1, V);
  for (unsigned OpN = 0; OpN < NumOps; OpN++)
    Inst->setOperand(OpN, Idxes[OpN]);
  return Inst;
}

IndexingInst *IndexingInst::get(Value *V, std::vector<size_t> &Idxes) {
  auto K = V->context();
  std::vector<Value *> IdxValues;
  for (auto Idx : Idxes) {
    IdxValues.push_back(ConstantInt::get(Idx, 32, K));
  }
  return IndexingInst::get(V, IdxValues);
}

bool IndexingInst::classof(const Value *V) {
  return V->op() == RT_IndexingInst;
}

Value *IndexingInst::val() const { return getOperand(-1); }

std::vector<Value *> IndexingInst::getIndices() const { return getOperands(); }

void IndexingInst::print(DiagnosticPrinter &Stream) const {
  Stream << *val();
  for (auto Idx : getIndices())
    Stream << "[" << *Idx << "]";
  Stream << std::endl;
}
}
