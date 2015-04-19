#include "rhine/IR.h"
#include "rhine/Context.h"
#include "rhine/LLVisitor.h"
#include "rhine/Externals.h"

namespace rhine {
//===--------------------------------------------------------------------===//
// LLVisitor visits.
//===--------------------------------------------------------------------===//
llvm::Type *LLVisitor::visit(IntegerType *V) {
  return RhBuilder.getInt32Ty();
}

llvm::Type *LLVisitor::visit(BoolType *V) {
  return RhBuilder.getInt1Ty();
}

llvm::Type *LLVisitor::visit(FloatType *V) {
  return RhBuilder.getFloatTy();
}

llvm::Type *LLVisitor::visit(StringType *V) {
  return RhBuilder.getInt8PtrTy();
}

llvm::Value *LLVisitor::visit(Symbol *V, Context *K) {
  assert(K && "null Symbol Table");
  return K->getMappingOrDie(V->getName());
}

llvm::Value *LLVisitor::visit(GlobalString *S) {
  auto SRef = llvm::StringRef(S->getVal());
  return RhBuilder.CreateGlobalStringPtr(SRef);
}

llvm::Constant *LLVisitor::visit(ConstantInt *I) {
  return llvm::ConstantInt::get(RhContext, APInt(32, I->getVal()));
}

llvm::Constant *LLVisitor::visit(ConstantBool *B) {
  return llvm::ConstantInt::get(RhContext, APInt(1, B->getVal()));
}

llvm::Constant *LLVisitor::visit(ConstantFloat *F) {
  return llvm::ConstantFP::get(RhContext, APFloat(F->getVal()));
}

llvm::Constant *LLVisitor::visit(Function *RhF, llvm::Module *M, Context *K) {
  auto FType = dyn_cast<FunctionType>(RhF->getType());
  auto RType = FType->getRTy()->toLL(M, K);
  std::vector<llvm::Type *> ArgTys;
  for (auto El: RhF->getArgumentList())
    ArgTys.push_back(El->getType()->toLL());
  auto ArgTyAr = makeArrayRef(ArgTys);
  auto F = llvm::Function::Create(llvm::FunctionType::get(RType, ArgTyAr, false),
                                  GlobalValue::ExternalLinkage,
                                  RhF->getName(), M);

  // Bind argument symbols to function argument values in symbol table
  auto S = RhF->getArgumentList().begin();
  auto V = F->arg_begin();
  auto End = RhF->getArgumentList().end();
  for (; S != End; ++S, ++V)
    K->addMapping((*S)->getName(), V);

  // Add function symbol to symbol table
  K->addMapping(RhF->getName(), F);

  BasicBlock *BB = BasicBlock::Create(rhine::RhContext, "entry", F);
  RhBuilder.SetInsertPoint(BB);
  llvm::Value *RhV = RhF->getVal()->toLL(M, K);
  RhBuilder.CreateRet(RhV);
  return F;
}

llvm::Value *LLVisitor::visit(AddInst *A) {
  auto Op0 = A->getOperand(0)->toLL();
  auto Op1 = A->getOperand(1)->toLL();
  return RhBuilder.CreateAdd(Op0, Op1);
}

llvm::Value *LLVisitor::visit(CallInst *C, llvm::Module *M, Context *K) {
  llvm::Function *Callee;
  if (auto Result = K->getMapping(C->getName()))
    Callee = dyn_cast<llvm::Function>(Result);
  else
    Callee = Externals::printf(M);

  auto Arg = C->getOperand(0);
  llvm::Value *StrPtr;
  if (auto Sym = dyn_cast<Symbol>(Arg))
    StrPtr = K->getMappingOrDie(Sym->getName());
  else
    StrPtr = Arg->toLL(M);

  return RhBuilder.CreateCall(Callee, StrPtr, C->getName());
}

void LLVisitor::visit(Module *RhM, llvm::Module *M, Context *K) {
  for (auto F: RhM->getVal())
    F->toLL(M, K);
}
}
