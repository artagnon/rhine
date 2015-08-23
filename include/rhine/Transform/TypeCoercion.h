//-*- C++ -*-
#ifndef RHINE_TYPECOERCION_H
#define RHINE_TYPECOERCION_H

#include "rhine/Pass/FunctionPass.h"

namespace rhine {
class FunctionType;
class ConstantInt;
class IntegerType;
class StringType;
class BoolType;
class Function;
class Value;
class User;
class Type;

class TypeCoercion : public FunctionPass {
  Context *K;
public:
  TypeCoercion();
  virtual ~TypeCoercion() {}
  Value *convertValue(ConstantInt *I, IntegerType *DestTy);
  Value *convertValue(Value *V, StringType *);
  Value *convertValue(Value *V, BoolType *);
  Value *convertValue(Value *V, Type *);
  void convertOperands(User *U, std::vector<Type *> Tys);
  void assertActualFormalCount(CallInst *Inst, FunctionType *FTy);
  void transformInstruction(Instruction *I);
  void transformUser(User *U);
  void runOnFunction(Function *F) override;
};
}

#endif
