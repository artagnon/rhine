#pragma once

#include "rhine/Pass/FunctionPass.hpp"
#include "rhine/Transform/ValueVisitor.hpp"

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
  Type *visit(Tensor *V) override;
  Type *visit(GlobalString *V) override;
  Type *visit(Prototype *V) override;
  Type *visit(BasicBlock *BB) override;
  Type *visit(Function *V) override;
  Type *visit(Pointer *V) override;
  Type *visit(BinaryArithInst *V) override;
  Type *visit(IfInst *V) override;
  Type *visit(Argument *V) override;
  Type *visit(BindInst *V) override;
  Type *visit(MallocInst *V) override;
  Type *visit(LoadInst *V) override;
  Type *visit(StoreInst *V) override;
  Type *visit(CallInst *V) override;
  Type *visit(ReturnInst *V) override;
  Type *visit(TerminatorInst *V) override;
  Type *visit(IndexingInst *V) override;

private:
  /// Visit all the instructions in a block possibly containing a branch
  /// instruction (hence visiting the branch blocks also), but not the merge
  /// block. Typically called once for the header block and once for the merge
  /// block (which is possibly a header block possibly containing another branch
  /// instruction).
  Type *visitHeaderBlock(BasicBlock *BB);

  /// Follow a single function pointer type to get the contained type.
  FunctionType *followFcnPointer(Type *CalleeTy);

  /// Function pointers returning function pointers that return function
  /// pointers, and so on needs to be handled. We chase all these function
  /// pointer types until we end up with a non-function-pointer type.
  FunctionType *followFcnPointers(Value *Callee, Location Loc);

  /// CallInst::visit helper. Visits the callee and all the operands.
  void visitCalleeAndOperands(CallInst *V);

  /// Verify arity of a callsite
  void verifyArity(CallInst *V, FunctionType *Ty);
};
}
