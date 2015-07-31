//-*- C++ -*-
#ifndef RHINE_VALUEVISITOR_H
#define RHINE_VALUEVISITOR_H

#include "rhine/IR.h"

namespace rhine {
template <typename R>
class ValueVisitor {
protected:
  ~ValueVisitor() {}
  virtual R visit(Value *V) {
    if (auto C = dyn_cast<ConstantInt>(V)) { return visit(C); }
    if (auto C = dyn_cast<ConstantBool>(V)) { return visit(C); }
    if (auto C = dyn_cast<ConstantFloat>(V)) { return visit(C); }
    if (auto C = dyn_cast<GlobalString>(V)) { return visit(C); }
    if (auto C = dyn_cast<Function>(V)) { return visit(C); }
    if (auto C = dyn_cast<AddInst>(V)) { return visit(C); }
    if (auto C = dyn_cast<IfInst>(V)) { return visit(C); }
    if (auto C = dyn_cast<Argument>(V)) { return visit(C); }
    if (auto C = dyn_cast<MallocInst>(V)) { return visit(C); }
    if (auto C = dyn_cast<LoadInst>(V)) { return visit(C); }
    if (auto C = dyn_cast<CallInst>(V)) { return visit(C); }
    assert(0 && "Unknown ValueVisitor dispatch");
  }
  virtual R visit(ConstantInt *I) = 0;
  virtual R visit(ConstantBool *B) = 0;
  virtual R visit(ConstantFloat *F) = 0;
  virtual R visit(GlobalString *G) = 0;
  virtual R visit(Function *F) = 0;
  virtual R visit(AddInst *A) = 0;
  virtual R visit(IfInst *F) = 0;
  virtual R visit(Argument *A) = 0;
  virtual R visit(MallocInst *B) = 0;
  virtual R visit(LoadInst *S) = 0;
  virtual R visit(CallInst *C) = 0;
};
}

#endif
