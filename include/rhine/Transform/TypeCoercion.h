//-*- C++ -*-
#ifndef RHINE_TYPECOERCION_H
#define RHINE_TYPECOERCION_H

#include "rhine/IR.h"
#include "rhine/Transform/FunctionTransform.h"

namespace rhine {
class TypeCoercion : public FunctionTransform {
public:
  TypeCoercion(Context *K) : FunctionTransform(K) {}
  Value *convertValue(ConstantInt *I, IntegerType *DestTy);
  Value *convertValue(Value *V, StringType *);
  Value *convertValue(Value *V, BoolType *);
  Value *convertValue(Value *V, Type *);
  void runOnFunction(Function *F) override;
};
}

#endif
