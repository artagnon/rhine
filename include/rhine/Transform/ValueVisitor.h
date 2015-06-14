//-*- C++ -*-
#ifndef RHINE_VALUEVISITOR_H
#define RHINE_VALUEVISITOR_H

#include "rhine/IR.h"

namespace rhine {
template <typename R>
class ValueVisitor {
protected:
  ~ValueVisitor() {}
  virtual R visit(Value *V);
  virtual R visit(ConstantInt *I) = 0;
  virtual R visit(ConstantBool *B) = 0;
  virtual R visit(ConstantFloat *F) = 0;
  virtual R visit(GlobalString *G) = 0;
  virtual R visit(Function *F) = 0;
  virtual R visit(AddInst *A) = 0;
  virtual R visit(Symbol *S) = 0;
  virtual R visit(CallInst *C) = 0;
  virtual R visit(BindInst *B) = 0;
};
}

#endif
