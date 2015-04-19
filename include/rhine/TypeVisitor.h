//-*- C++ -*-

#include "rhine/IR.h"

namespace rhine {
struct TypeVisitor
{
  static Symbol *visit(Symbol *V, Context *K);
  static GlobalString *visit(GlobalString *S);
  static ConstantInt *visit(ConstantInt *I);
  static ConstantBool *visit(ConstantBool *B);
  static ConstantFloat *visit(ConstantFloat *F);
  static Function *visit(Function *RhF, Context *K);
  static AddInst *visit(AddInst *A);
  static CallInst *visit(CallInst *C, Context *K);
};
}
