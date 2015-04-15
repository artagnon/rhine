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
  auto RhRType = F->getVal()->getType();
  auto RType = RhRType->toLL(M, K);
  std::vector<llvm::Type *> ArgTys;
  for (auto El: F->getArgumentList())
    ArgTys.push_back(El->getType()->toLL());
  auto ArgTyAr = makeArrayRef(ArgTys);
  auto F = llvm::Function::Create(llvm::FunctionType::get(RType, ArgTyAr, false),
                                  GlobalValue::ExternalLinkage,
                                  F->getName(), M);

  // Bind argument symbols to function argument values in symbol table
  auto S = F->getArgumentList().begin();
  auto V = F->arg_begin();
  auto End = F->getArgumentList().end();
  for (; S != End; ++S, ++V)
    K->addMapping((*S)->getName(), V);

  // Add function symbol to symbol table
  K->addMapping(F->getName(), F);

  BasicBlock *BB = BasicBlock::Create(rhine::RhContext, "entry", F);
  RhBuilder.SetInsertPoint(BB);
  Type *RhV = F->getVal()->toLL(M, K);
  RhBuilder.CreateRet(RhV);
  return F;
}

Type *TypeVisitor::visit(AddInst *A) {
  return IntegerType;
}

Type *TypeVisitor::visit(CallInst *C, Context *K) {
  llvm::Function *Callee;
  if (auto Result = K->getMapping(C->getName()))
    Callee = dyn_cast<llvm::Function>(Result);
  else
    Callee = Externals::printf(M);

  auto Arg = C->getOperand(0);
  Type *StrPtr;
  if (auto Sym = dynamic_cast<Symbol *>(Arg))
    StrPtr = K->getMappingOrDie(Sym->getName());
  else
    StrPtr = Arg->toLL(M);

  return RhBuilder.CreateCall(Callee, StrPtr, C->getName());
}

void TypeVisitor::visit(Context *K) {
  for (auto F: M->getVal())
    F->typeInfer(K);
}
}
