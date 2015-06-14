//-*- C++ -*-
#ifndef RHINE_TYPEINFER_H
#define RHINE_TYPEINFER_H

#include "rhine/Transform/ValueVisitor.h"

namespace rhine {
class TypeInfer : public ValueVisitor<Type *> {
  Context *K;
public:
  TypeInfer(Context *K_) : K(K_) {}
  ~TypeInfer() {}
  void runOnModule(Module *M);
  template <typename T>
  Type *typeInferValueList(std::vector<T> V);
  using ValueVisitor<Type *>::visit;
  Type *visit(ConstantInt *I) override;
  Type *visit(ConstantBool *B) override;
  Type *visit(ConstantFloat *F) override;
  Type *visit(GlobalString *G) override;
  Type *visit(Function *F) override;
  Type *visit(AddInst *A) override;
  Type *visit(Symbol *S) override;
  Type *visit(CallInst *C) override;
  Type *visit(BindInst *B) override;
};
}

#endif
