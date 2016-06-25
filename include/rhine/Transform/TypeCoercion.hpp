//-*- C++ -*-
#ifndef RHINE_TYPECOERCION_H
#define RHINE_TYPECOERCION_H

#include "rhine/Pass/FunctionPass.hpp"

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
  virtual ~TypeCoercion() = default;

  /// These overloadeded instances convert the source value to the destination
  /// type, via coercion.
  Value *convertValue(ConstantInt *I, IntegerType *DestTy);
  Value *convertValue(Value *V, StringType *);
  Value *convertValue(Value *V, BoolType *);
  Value *convertValue(Value *V, Type *);

  /// Convert operands of a User to the types in Tys
  void convertOperands(User *U, std::vector<Type *> Tys);

  /// Match CallInst, IfInst etc. for coercion
  void transformInstruction(Instruction *I);

  /// This is a FunctionPass
  void runOnFunction(Function *F) override;
};
}

#endif
