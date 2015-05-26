//-*- C++ -*-
#ifndef RHINE_TYPECOERCION_H
#define RHINE_TYPECOERCION_H

#include "rhine/IR.h"

namespace rhine {
class TypeCoercion {
  Context *K;
public:
  TypeCoercion(Context *K) : K(K) {}
  Value *convertValue(ConstantInt *I, IntegerType *DestTy);
  Value *convertValue(ConstantInt *I, StringType *);
  Value *convertValue(Value *V, Type *);
  void runOnFunction(Function *F);
  void runOnModule(Module *M);
};
}

#endif
