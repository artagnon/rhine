//-*- C++ -*-
#ifndef RHINE_TYPEINFER_H
#define RHINE_TYPEINFER_H

#include "rhine/Transform/ValueVisitor.h"
#include "rhine/Pass/FunctionPass.h"

namespace rhine {
class TypeInfer : public ValueVisitor<Type *>, public FunctionPass {
  Context *K;
public:
  TypeInfer();
  virtual ~TypeInfer() {}
  void runOnFunction(Function *F) override;
  using ValueVisitor<Type *>::visit;
  Type *visit(ConstantInt *V) override;
  Type *visit(ConstantBool *V) override;
  Type *visit(ConstantFloat *V) override;
  Type *visit(GlobalString *V) override;
  Type *visit(Prototype *V) override;
  Type *visit(BasicBlock *BB) override;
  Type *visit(Function *V) override;
  Type *visit(Pointer *V) override;
  Type *visit(AddInst *V) override;
  Type *visit(SubInst *V) override;
  Type *visit(IfInst *V) override;
  Type *visit(Argument *V) override;
  Type *visit(MallocInst *V) override;
  Type *visit(LoadInst *V) override;
  Type *visit(StoreInst *V) override;
  Type *visit(CallInst *V) override;
  Type *visit(ReturnInst *V) override;
  Type *visit(TerminatorInst *V) override;
};
}

#endif
