#include "rhine/IR.h"
#include "rhine/TypeVisitor.h"
#include <iostream>

namespace rhine {
//===--------------------------------------------------------------------===//
// TypeVisitor visits.
//===--------------------------------------------------------------------===//
Symbol *TypeVisitor::visit(Symbol *V, Context *K) {
  Symbol *NewV = V;
  if (V->getType() == Type::get()) {
    NewV = K->getNameMapping(V->getName());
    assert (NewV->getType() != Type::get() && "Missing seed to infer type");
  }
  K->addNameMapping(NewV->getName(), NewV);
  return NewV;
}

GlobalString *TypeVisitor::visit(GlobalString *S) {
  return S;
}

ConstantInt *TypeVisitor::visit(ConstantInt *I) {
  return I;
}

ConstantBool *TypeVisitor::visit(ConstantBool *B) {
  return B;
}

ConstantFloat *TypeVisitor::visit(ConstantFloat *F) {
  return F;
}

Function *TypeVisitor::visit(Function *F, Context *K) {
  for (auto &El: F->getArgumentList())
    El->typeInfer(K);
  auto FType = dynamic_cast<FunctionType *>(F->getType());
  FType->setRTy(F->getVal()->typeInfer(K));
  return F;
}

AddInst *TypeVisitor::visit(AddInst *A) {
  auto LType = A->getOperand(0)->typeInfer();
  if (A->getOperand(1)->typeInfer() != LType)
    assert(0 && "AddInst with operands of different types");
  return A;
}

CallInst *TypeVisitor::visit(CallInst *C, Context *K) {
  return C;
}
}
