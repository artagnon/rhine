#include "rhine/IR.h"
#include "rhine/TypeVisitor.h"

namespace rhine {
//===--------------------------------------------------------------------===//
// TypeVisitor visits.
//===--------------------------------------------------------------------===//
Type *TypeVisitor::visit(Symbol *V, Context *K) {
  auto ThisType = V->getType();
  if (ThisType == Type::get())
    assert(0 && "Symbol type inference not yet supported");
  return ThisType;
}

Type *TypeVisitor::visit(GlobalString *S) {
  return S->getType();
}

Type *TypeVisitor::visit(ConstantInt *I) {
  return I->getType();
}

Type *TypeVisitor::visit(ConstantBool *B) {
  return B->getType();
}

Type *TypeVisitor::visit(ConstantFloat *F) {
  return F->getType();
}

Type *TypeVisitor::visit(Function *F, Context *K) {
  return F->getVal()->typeInfer(K);
}

Type *TypeVisitor::visit(AddInst *A) {
  auto LType = A->getOperand(0)->typeInfer();
  if (A->getOperand(1)->typeInfer() != LType)
    assert(0 && "AddInst with operands of different types");
  return LType;
}

Type *TypeVisitor::visit(CallInst *C, Context *K) {
  return C->getType();
}

void TypeVisitor::visit(Module *M, Context *K) {
  for (auto F: M->getVal())
    F->typeInfer(K);
}
}
