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
Type *Symbol::typeInfer(Context *K) {
  return TypeVisitor::visit(this, K)->getType();
}

Type *rhine::ConstantInt::typeInfer(Context *K) {
  return TypeVisitor::visit(this)->getType();
}

Type *ConstantBool::typeInfer(Context *K) {
  return TypeVisitor::visit(this)->getType();
}

Type *ConstantFloat::typeInfer(Context *K) {
  return TypeVisitor::visit(this)->getType();
}

Type *GlobalString::typeInfer(Context *K) {
  return TypeVisitor::visit(this)->getType();
}

Type *Function::typeInfer(Context *K) {
  return TypeVisitor::visit(this, K)->getType();
}

Type *AddInst::typeInfer(Context *K) {
  return TypeVisitor::visit(this)->getType();
}

Type *CallInst::typeInfer(Context *K) {
  return TypeVisitor::visit(this, K)->getType();
}

void Module::typeInfer(Context *K) {
  auto V = this->getVal();
  std::transform(V.begin(), V.end(), V.begin(),
                 [K](Function *F) -> Function * {
                   return TypeVisitor::visit(F, K);
                 });
  this->setVal(V);
}
}
