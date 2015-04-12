#include "rhine/IR.h"
#include "rhine/LLVisitor.h"
#include "rhine/Externals.h"

namespace rhine {
//===--------------------------------------------------------------------===//
// ToLL() stubs.
//===--------------------------------------------------------------------===//
llvm::Type *IntegerType::toLL(llvm::Module *M, SymbolTable *K) {
  return LLVisitor::visit(this);
}

llvm::Type *BoolType::toLL(llvm::Module *M, SymbolTable *K) {
  return LLVisitor::visit(this);
}

llvm::Type *FloatType::toLL(llvm::Module *M, SymbolTable *K) {
  return LLVisitor::visit(this);
}

llvm::Type *StringType::toLL(llvm::Module *M, SymbolTable *K) {
  return LLVisitor::visit(this);
}

llvm::Type *FunctionType::toLL(llvm::Module *M, SymbolTable *K) {
  return nullptr;
}

template <typename T>
llvm::Type *ArrayType<T>::toLL(llvm::Module *M, SymbolTable *K) {
  return nullptr;
}

llvm::Constant *rhine::ConstantInt::toLL(llvm::Module *M, SymbolTable *K) {
  return LLVisitor::visit(this);
}

llvm::Constant *ConstantBool::toLL(llvm::Module *M, SymbolTable *K) {
  return LLVisitor::visit(this);
}

llvm::Constant *ConstantFloat::toLL(llvm::Module *M, SymbolTable *K) {
  return LLVisitor::visit(this);
}

llvm::Value *GlobalString::toLL(llvm::Module *M, SymbolTable *K) {
  return LLVisitor::visit(this);
}

llvm::Constant *Function::toLL(llvm::Module *M, SymbolTable *K) {
  return LLVisitor::visit(this, M, K);
}

llvm::Value *Symbol::toLL(llvm::Module *M, SymbolTable *K) {
  return LLVisitor::visit(this, K);
}

llvm::Value *AddInst::toLL(llvm::Module *M, SymbolTable *K) {
  return LLVisitor::visit(this);
}

llvm::Value *CallInst::toLL(llvm::Module *M, SymbolTable *K) {
  return LLVisitor::visit(this, M);
}

void Module::toLL(llvm::Module *M, SymbolTable *K) {
  return LLVisitor::visit(this, M, K);
}


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

llvm::Value *LLVisitor::visit(Symbol *V, SymbolTable *K) {
  assert(K && "null Symbol Table");
  auto Val = K->getMapping(V);
  assert(Val && "Unbound symbol");
  return Val;
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

llvm::Constant *LLVisitor::visit(Function *RhF, llvm::Module *M, SymbolTable *K) {
  auto RType = RhF->getVal()->getType()->toLL(M, K);
  std::vector<llvm::Type *> ArgTys;
  for (auto El: RhF->getArgumentList())
    ArgTys.push_back(El->getType()->toLL());
  auto ArgTyAr = makeArrayRef(ArgTys);
  auto F = llvm::Function::Create(llvm::FunctionType::get(RType, ArgTyAr, false),
                                  GlobalValue::ExternalLinkage,
                                  RhF->getName(), M);

  // Bind argument symbols to function argument values
  auto S = RhF->getArgumentList().begin();
  auto V = F->arg_begin();
  auto End = RhF->getArgumentList().end();
  for (; S != End; ++S, ++V)
    K->addMapping(*S, V);

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

llvm::Value *LLVisitor::visit(CallInst *C, llvm::Module *M) {
  auto Callee = Externals::printf(M);
  auto StrPtr = C->getOperand(0)->toLL(M);
  return RhBuilder.CreateCall(Callee, StrPtr, C->getName());
}

void LLVisitor::visit(Module *RhM, llvm::Module *M, SymbolTable *K) {
  for (auto F: RhM->getVal())
    F->toLL(M, K);
}
}
