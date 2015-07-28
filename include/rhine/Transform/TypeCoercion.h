//-*- C++ -*-
#ifndef RHINE_TYPECOERCION_H
#define RHINE_TYPECOERCION_H

#include "rhine/IR.h"
#include "rhine/Pass/FunctionPass.h"

namespace rhine {
class TypeCoercion : public FunctionPass {
public:
  TypeCoercion(Context *K) : FunctionPass(K) {}
  virtual ~TypeCoercion() {}
  Value *convertValue(ConstantInt *I, IntegerType *DestTy);
  Value *convertValue(Value *V, StringType *);
  Value *convertValue(Value *V, BoolType *);
  Value *convertValue(Value *V, Type *);
  void runOnFunction(Function *F) override;
};
}

#endif
