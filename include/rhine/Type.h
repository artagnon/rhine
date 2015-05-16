//-*- C++ -*-

#ifndef TYPE_H
#define TYPE_H

/// Discriminator for LLVM-style RTTI (dyn_cast<> et al.)
enum RTType {
  RT_UnType,
  RT_VoidType,
  RT_IntegerType,
  RT_BoolType,
  RT_FloatType,
  RT_StringType,
  RT_FunctionType,
};

enum RTValue {
  RT_Symbol,
  RT_GlobalString,
  RT_ConstantInt,
  RT_ConstantBool,
  RT_ConstantFloat,
  RT_Function,
  RT_AddInst,
  RT_CallInst,
  RT_BindInst,
};

#endif
