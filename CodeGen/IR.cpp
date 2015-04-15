#include "rhine/IR.h"
#include "rhine/LLVisitor.h"
#include "rhine/TypeVisitor.h"
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

template <typename T>
llvm::Type *ArrayType<T>::toLL(llvm::Module *M, Context *K) {
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

void Module::toLL(llvm::Module *M, Context *K) {
  return LLVisitor::visit(this, M, K);
}

//===--------------------------------------------------------------------===//
// typeInfer() stubs.
//===--------------------------------------------------------------------===//
void rhine::ConstantInt::typeInfer(Context *K) {
  TypeVisitor::visit(this);
}

void ConstantBool::typeInfer(Context *K) {
  TypeVisitor::visit(this);
}

void ConstantFloat::typeInfer(Context *K) {
  TypeVisitor::visit(this);
}

void GlobalString::typeInfer(Context *K) {
  TypeVisitor::visit(this);
}

void Function::typeInfer(Context *K) {
  TypeVisitor::visit(this, K);
}

void Symbol::typeInfer(Context *K) {
  TypeVisitor::visit(this, K);
}

void AddInst::typeInfer(Context *K) {
  TypeVisitor::visit(this);
}

void CallInst::typeInfer(Context *K) {
  TypeVisitor::visit(this, K);
}

void Module::typeInfer(Context *K) {
  TypeVisitor::visit(this, K);
}
}
