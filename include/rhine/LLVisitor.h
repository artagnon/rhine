//-*- C++ -*-

#ifndef LLVISITOR_H
#define LLVISITOR_H

#include "rhine/IR.h"

namespace rhine {
class LLVisitor
{
public:
  static llvm::Type *visit(IntegerType *V);
  static llvm::Type *visit(BoolType *V);
  static llvm::Type *visit(FloatType *V);
  static llvm::Type *visit(StringType *V);
  static llvm::Value *visit(Symbol *V, Context *K);
  static llvm::Value *visit(GlobalString *S);
  static llvm::Constant *visit(ConstantInt *I);
  static llvm::Constant *visit(ConstantBool *B);
  static llvm::Constant *visit(ConstantFloat *F);
  static llvm::Constant *visit(Function *RhF, llvm::Module *M, Context *K);
  static llvm::Value *visit(AddInst *A);
  static llvm::Value *visit(CallInst *C, llvm::Module *M);
  static void visit(Module *RhM, llvm::Module *M, Context *K);
};
}

#endif
