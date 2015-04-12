#include "rhine/IR.h"
#include "rhine/Context.h"

using namespace rhine;

namespace rhine {
Function *emitAdd2Const() {
  auto FTy = FunctionType::get(IntegerType::get());
  auto F = Function::get(FTy);
  auto Op = AddInst::get(IntegerType::get());
  auto I1 = ConstantInt::get(3);
  auto I2 = ConstantInt::get(4);
  Op->addOperand(I1);
  Op->addOperand(I2);
  F->setName("foom");
  F->setBody(std::vector<Value *> { Op });
  return F;
}

Function *untypedAdd() {
  auto Ctx = Context();
  auto FTy = FunctionType::get(IntegerType::get(), Type::get(), Type::get());
  auto F = Function::get(FTy);
  auto Op = AddInst::get(Type::get());
  auto I1 = Symbol::get("untypedvar", Type::get(), &Ctx);
  auto I2 = ConstantInt::get(4);
  Op->addOperand(I1);
  Op->addOperand(I2);
  F->setName("foom");
  F->setBody(std::vector<Value *> { Op });
  return F;
}
}
