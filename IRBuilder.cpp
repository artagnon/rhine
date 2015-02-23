#include "rhine/Ast.h"

using namespace rhine;

namespace rhine {
Function *emitAdd2Const(llvm::Module *M) {
  auto FTy = FunctionType::get(IntegerType::get());
  auto F = Function::get(M, FTy);
  auto Op = AddInst::get(IntegerType::get());
  auto I1 = ConstantInt::get(3);
  auto I2 = ConstantInt::get(4);
  Op->addOperand(I1);
  Op->addOperand(I2);
  F->setName("foom");
  F->setBody(Op);
  return F;
}

Function *untypedAdd(llvm::Module *M) {
  auto FTy = FunctionType::get(IntegerType::get(), Type::get(), Type::get());
  auto F = Function::get(M, FTy);
  auto Op = AddInst::get(Type::get());
  auto I1 = Variable::get("untypedvar");
  auto I2 = ConstantInt::get(4);
  Op->addOperand(I1);
  Op->addOperand(I2);
  F->setName("foom");
  F->setBody(Op);
  return F;
}
}
