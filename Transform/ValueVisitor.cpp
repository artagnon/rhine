#include "rhine/Transform/ValueVisitor.h"
#include "rhine/IR.h"

namespace rhine {
template <>
Type *ValueVisitor<Type *>::visit(Value *V) {
  if (auto C = dyn_cast<ConstantInt>(V)) { return visit(C); }
  if (auto C = dyn_cast<ConstantBool>(V)) { return visit(C); }
  if (auto C = dyn_cast<ConstantFloat>(V)) { return visit(C); }
  if (auto C = dyn_cast<GlobalString>(V)) { return visit(C); }
  if (auto C = dyn_cast<Function>(V)) { return visit(C); }
  if (auto C = dyn_cast<AddInst>(V)) { return visit(C); }
  if (auto C = dyn_cast<Symbol>(V)) { return visit(C); }
  if (auto C = dyn_cast<CallInst>(V)) { return visit(C); }
  if (auto C = dyn_cast<BindInst>(V)) { return visit(C); }
  assert(0 && "Unknown ValueVisitor dispatch");
}
}
