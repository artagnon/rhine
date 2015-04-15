//-*- C++ -*-

#include "rhine/IR.h"

namespace rhine {
class TypeVisitor
{
public:
  static Type *visit(Symbol *V, Context *K);
  static Type *visit(GlobalString *S);
  static Type *visit(ConstantInt *I);
  static Type *visit(ConstantBool *B);
  static Type *visit(ConstantFloat *F);
  static Type *visit(Function *RhF, Context *K);
  static Type *visit(AddInst *A);
  static Type *visit(CallInst *C, Context *K);
  static void visit(Module *RhM, Context *K);
};
}
