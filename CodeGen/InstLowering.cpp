#include "rhine/Context.h"
#include "rhine/IR/Instruction.h"
#include "rhine/Externals.h"

namespace rhine {
llvm::Value *lookupOrLower(Value *V, llvm::Module *M, Context *K) {
  if (auto Sym = dyn_cast<Symbol>(V))
    return K->getMappingVal(Sym->getName(), Sym->getSourceLocation());
  else
    return V->toLL(M, K);
}

llvm::Value *CallInst::toLL(llvm::Module *M, Context *K) {
  location SourceLoc = getSourceLocation();
  llvm::Function *Callee;
  auto Name = getName();
  if (auto Result = K->getMappingVal(Name)) {
    if (auto CalleeCandidate = dyn_cast<llvm::Function>(Result))
      Callee = CalleeCandidate;
    else {
      K->DiagPrinter->errorReport(
          SourceLoc, Name + " was not declared as a function");
      exit(1);
    }
  } else if (auto FPtr = Externals::get()->getMapping(Name)) {
    if (auto CalleeCandidate = dyn_cast<llvm::Function>(FPtr(M, K, SourceLoc)))
      Callee = CalleeCandidate;
    else {
      // Polymorphic externals?
      K->DiagPrinter->errorReport(
          SourceLoc, Name + " was declared with different signature earlier");
      exit(1);
    }
  } else {
      K->DiagPrinter->errorReport(
          SourceLoc, "unable to look up function " + Name);
      exit(1);
  }

  // Extract Callee's argument types
  auto TargetFnTy = dyn_cast<llvm::FunctionType>(
      Callee->getType()->getElementType());
  std::vector<llvm::Type *> TargetArgumentTys;
  for (auto I = TargetFnTy->param_begin(); I != TargetFnTy->param_end(); ++I)
    TargetArgumentTys.push_back(*I);

  // Integer bitwidth refinement
  auto Arg = getOperand(0);
  if (Arg->getType()->getTyID() == RT_IntegerType) {
    auto W = dyn_cast<llvm::IntegerType>(TargetArgumentTys[0])->getBitWidth();
    Arg->setType(IntegerType::get(W, K));
  }

  // Lowering argument
  auto ArgLL = lookupOrLower(Arg, M, K);
  return K->Builder->CreateCall(Callee, ArgLL, getName());
}

llvm::Value *AddInst::toLL(llvm::Module *M, Context *K) {
  auto Op0 = getOperand(0)->toLL(M, K);
  auto Op1 = getOperand(1)->toLL(M, K);
  return K->Builder->CreateAdd(Op0, Op1);
}

llvm::Value *BindInst::toLL(llvm::Module *M, Context *K) {
  auto V = getVal()->toLL(M, K);
  K->addMapping(getName(), nullptr, V);
  return nullptr;
}
}
