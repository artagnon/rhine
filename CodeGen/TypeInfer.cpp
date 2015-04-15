#include "rhine/IR.h"
#include "rhine/TypeVisitor.h"

namespace rhine {
//===--------------------------------------------------------------------===//
// TypeVisitor visits.
//===--------------------------------------------------------------------===//
Type *TypeVisitor::visit(Symbol *V, Context *K) {
  assert(0 && "Symbol type inference not yet supported");
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
  return F->getType();
}

Type *TypeVisitor::visit(AddInst *A) {
  return A->getType();
}

Type *TypeVisitor::visit(CallInst *C, Context *K) {
  return C->getType();
}

void TypeVisitor::visit(Module *M, Context *K) {
}
}
