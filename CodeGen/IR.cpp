#include "rhine/IR.h"
#include "rhine/LLVisitor.h"
#include "rhine/Context.h"

namespace rhine {
//===--------------------------------------------------------------------===//
// ToLL() stubs.
//===--------------------------------------------------------------------===//
llvm::Type *IntegerType::toLL(llvm::Module *M, Context *K) {
  return LLVisitor::visit(this);
}

llvm::Type *BoolType::toLL(llvm::Module *M, Context *K) {
  return LLVisitor::visit(this);
}

llvm::Type *FloatType::toLL(llvm::Module *M, Context *K) {
  return LLVisitor::visit(this);
}

llvm::Type *StringType::toLL(llvm::Module *M, Context *K) {
  return LLVisitor::visit(this);
}

llvm::Type *FunctionType::toLL(llvm::Module *M, Context *K) {
  return nullptr;
}

llvm::Constant *rhine::ConstantInt::toLL(llvm::Module *M, Context *K) {
  return LLVisitor::visit(this);
}

llvm::Constant *ConstantBool::toLL(llvm::Module *M, Context *K) {
  return LLVisitor::visit(this);
}

llvm::Constant *ConstantFloat::toLL(llvm::Module *M, Context *K) {
  return LLVisitor::visit(this);
}

llvm::Value *GlobalString::toLL(llvm::Module *M, Context *K) {
  return LLVisitor::visit(this);
}

llvm::Constant *Function::toLL(llvm::Module *M, Context *K) {
  return LLVisitor::visit(this, M, K);
}

llvm::Value *Symbol::toLL(llvm::Module *M, Context *K) {
  return LLVisitor::visit(this, K);
}

llvm::Value *AddInst::toLL(llvm::Module *M, Context *K) {
  return LLVisitor::visit(this);
}

llvm::Value *CallInst::toLL(llvm::Module *M, Context *K) {
  return LLVisitor::visit(this, M, K);
}

llvm::Value *BindInst::toLL(llvm::Module *M, Context *K) {
  return nullptr;
}

void Module::toLL(llvm::Module *M, Context *K) {
  return LLVisitor::visit(this, M, K);
}
}
