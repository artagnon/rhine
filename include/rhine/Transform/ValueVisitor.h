//-*- C++ -*-
#ifndef RHINE_VALUEVISITOR_H
#define RHINE_VALUEVISITOR_H

#include "rhine/IR/Value.h"
#include "rhine/IR/Instruction.h"
#include "rhine/IR/Constant.h"
#include "rhine/IR/GlobalValue.h"

namespace rhine {
class ConstantInt;
class ConstantBool;
class ConstantFloat;
class GlobalString;
class Function;
class Prototype;
class AddInst;
class SubInst;
class IfInst;
class Argument;
class MallocInst;
class LoadInst;
class CallInst;
class ReturnInst;
class TerminatorInst;

template <typename R>
class ValueVisitor {
protected:
  virtual ~ValueVisitor() {}
  virtual R visit(Value *V) {
    switch(V->getValID()) {
    case RT_ConstantInt: return visit(cast<ConstantInt>(V)); break;
    case RT_ConstantBool: return visit(cast<ConstantBool>(V)); break;
    case RT_ConstantFloat: return visit(cast<ConstantFloat>(V)); break;
    case RT_GlobalString: return visit(cast<GlobalString>(V)); break;
    case RT_Prototype: return visit(cast<Prototype>(V)); break;
    case RT_Function: return visit(cast<Function>(V)); break;
    case RT_Pointer: return visit(cast<Pointer>(V)); break;
    case RT_AddInst: return visit(cast<AddInst>(V)); break;
    case RT_SubInst: return visit(cast<SubInst>(V)); break;
    case RT_IfInst: return visit(cast<IfInst>(V)); break;
    case RT_Argument: return visit(cast<Argument>(V)); break;
    case RT_MallocInst: return visit(cast<MallocInst>(V)); break;
    case RT_LoadInst: return visit(cast<LoadInst>(V)); break;
    case RT_StoreInst: return visit(cast<StoreInst>(V)); break;
    case RT_CallInst: return visit(cast<CallInst>(V)); break;
    case RT_ReturnInst: return visit(cast<ReturnInst>(V)); break;
    case RT_TerminatorInst: return visit(cast<TerminatorInst>(V)); break;
    case RT_BasicBlock: return visit(cast<BasicBlock>(V)); break;
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
  virtual R visit(SubInst *A) = 0;
  virtual R visit(IfInst *F) = 0;
  virtual R visit(Argument *A) = 0;
  virtual R visit(MallocInst *B) = 0;
  virtual R visit(LoadInst *S) = 0;
  virtual R visit(StoreInst *S) = 0;
  virtual R visit(CallInst *C) = 0;
  virtual R visit(ReturnInst *C) = 0;
  virtual R visit(TerminatorInst *C) = 0;
  virtual R visit(BasicBlock *BB) = 0;
};
}

#endif
