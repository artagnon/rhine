#pragma once

#include "rhine/IR/Constant.hpp"
#include "rhine/IR/GlobalValue.hpp"
#include "rhine/IR/Instruction.hpp"
#include "rhine/IR/Tensor.hpp"
#include "rhine/IR/Value.hpp"

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
class IndexingInst;

template <typename R> class ValueVisitor {
protected:
  virtual ~ValueVisitor() = default;
  virtual R visit(Value *V) {
    switch (V->op()) {
    case RT_ConstantInt:
      return visit(cast<ConstantInt>(V));
    case RT_ConstantBool:
      return visit(cast<ConstantBool>(V));
    case RT_ConstantFloat:
      return visit(cast<ConstantFloat>(V));
    case RT_Tensor:
      return visit(cast<Tensor>(V));
    case RT_GlobalString:
      return visit(cast<GlobalString>(V));
    case RT_Prototype:
      return visit(cast<Prototype>(V));
    case RT_Function:
      return visit(cast<Function>(V));
    case RT_Pointer:
      return visit(cast<Pointer>(V));
    case RT_AddInst:
    case RT_SubInst:
    case RT_MulInst:
    case RT_DivInst:
      return visit(cast<BinaryArithInst>(V));
    case RT_IfInst:
      return visit(cast<IfInst>(V));
    case RT_Argument:
      return visit(cast<Argument>(V));
    case RT_BindInst:
      return visit(cast<BindInst>(V));
    case RT_MallocInst:
      return visit(cast<MallocInst>(V));
    case RT_LoadInst:
      return visit(cast<LoadInst>(V));
    case RT_StoreInst:
      return visit(cast<StoreInst>(V));
    case RT_CallInst:
      return visit(cast<CallInst>(V));
    case RT_ReturnInst:
      return visit(cast<ReturnInst>(V));
    case RT_TerminatorInst:
      return visit(cast<TerminatorInst>(V));
    case RT_IndexingInst:
      return visit(cast<IndexingInst>(V));
    case RT_BasicBlock:
      return visit(cast<BasicBlock>(V));
    case RT_UnresolvedValue:
      assert(0 && "UnresolvedValue hit");
    default:
      assert(0 && "Unknown ValueVisitor dispatch");
    }
    return nullptr;
  }
  virtual R visit(ConstantInt *I) = 0;
  virtual R visit(ConstantBool *B) = 0;
  virtual R visit(ConstantFloat *F) = 0;
  virtual R visit(Tensor *T) = 0;
  virtual R visit(GlobalString *G) = 0;
  virtual R visit(Prototype *P) = 0;
  virtual R visit(Function *F) = 0;
  virtual R visit(Pointer *P) = 0;
  virtual R visit(BinaryArithInst *B) = 0;
  virtual R visit(IfInst *F) = 0;
  virtual R visit(Argument *A) = 0;
  virtual R visit(BindInst *B) = 0;
  virtual R visit(MallocInst *B) = 0;
  virtual R visit(LoadInst *S) = 0;
  virtual R visit(StoreInst *S) = 0;
  virtual R visit(CallInst *C) = 0;
  virtual R visit(ReturnInst *C) = 0;
  virtual R visit(TerminatorInst *C) = 0;
  virtual R visit(IndexingInst *C) = 0;
  virtual R visit(BasicBlock *BB) = 0;
};
}
