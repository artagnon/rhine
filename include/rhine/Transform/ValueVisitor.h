//-*- C++ -*-
#ifndef RHINE_VALUEVISITOR_H
#define RHINE_VALUEVISITOR_H

#include "rhine/IR/Value.h"

namespace rhine {
class ConstantInt;
class ConstantBool;
class ConstantFloat;
class GlobalString;
class Function;
class Prototype;
class AddInst;
class IfInst;
class Argument;
class MallocInst;
class LoadInst;
class CallInst;
class ReturnInst;

template <typename R>
class ValueVisitor {
protected:
  ~ValueVisitor() {}
  virtual R visit(Value *V) {
    switch(V->getValID()) {
    case RT_ConstantInt: return visit(dyn_cast<ConstantInt>(V)); break;
    case RT_ConstantBool: return visit(dyn_cast<ConstantBool>(V)); break;
    case RT_ConstantFloat: return visit(dyn_cast<ConstantFloat>(V)); break;
    case RT_GlobalString: return visit(dyn_cast<GlobalString>(V)); break;
    case RT_Prototype: return visit(dyn_cast<Prototype>(V)); break;
    case RT_Function: return visit(dyn_cast<Function>(V)); break;
    case RT_Pointer: return visit(dyn_cast<Pointer>(V)); break;
    case RT_AddInst: return visit(dyn_cast<AddInst>(V)); break;
    case RT_IfInst: return visit(dyn_cast<IfInst>(V)); break;
    case RT_Argument: return visit(dyn_cast<Argument>(V)); break;
    case RT_MallocInst: return visit(dyn_cast<MallocInst>(V)); break;
    case RT_LoadInst: return visit(dyn_cast<LoadInst>(V)); break;
    case RT_StoreInst: return visit(dyn_cast<StoreInst>(V)); break;
    case RT_CallInst: return visit(dyn_cast<CallInst>(V)); break;
    case RT_ReturnInst: return visit(dyn_cast<ReturnInst>(V)); break;
    default: assert(0 && "Unknown ValueVisitor dispatch");
    }
  }
  virtual R visit(ConstantInt *I) = 0;
  virtual R visit(ConstantBool *B) = 0;
  virtual R visit(ConstantFloat *F) = 0;
  virtual R visit(GlobalString *G) = 0;
  virtual R visit(Prototype *P) = 0;
  virtual R visit(Function *F) = 0;
  virtual R visit(Pointer *P) = 0;
  virtual R visit(AddInst *A) = 0;
  virtual R visit(IfInst *F) = 0;
  virtual R visit(Argument *A) = 0;
  virtual R visit(MallocInst *B) = 0;
  virtual R visit(LoadInst *S) = 0;
  virtual R visit(StoreInst *S) = 0;
  virtual R visit(CallInst *C) = 0;
  virtual R visit(ReturnInst *C) = 0;
};
}

#endif
