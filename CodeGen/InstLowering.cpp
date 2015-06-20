#include "rhine/Context.h"
#include "rhine/IR/Instruction.h"
#include "rhine/Externals.h"

namespace rhine {
llvm::Value *getCalleeFunction(std::string Name, location SourceLoc,
                               llvm::Module *M, Context *K) {
  if (auto Result = K->getMappingVal(Name)) {
    if (auto CalleeCandidate = dyn_cast<llvm::Function>(Result))
      return CalleeCandidate;
    else if (auto PTy = dyn_cast<llvm::PointerType>(Result->getType())) {
      if (dyn_cast<llvm::FunctionType>(PTy->getElementType()))
        return Result;
    } else {
      K->DiagPrinter->errorReport(
          SourceLoc, Name + " was not declared as a function");
      exit(1);
    }
  } else if (auto FCandidate = Externals::get(K)->getMappingVal(Name, M)) {
    if (auto CalleeCandidate = dyn_cast<llvm::Function>(FCandidate))
      return CalleeCandidate;
    else {
      // Polymorphic externals?
      K->DiagPrinter->errorReport(
          SourceLoc, Name + " was declared with different signature earlier");
      exit(1);
    }
  }
  K->DiagPrinter->errorReport(
      SourceLoc, "unable to look up function " + Name);
  exit(1);
}

llvm::Value *CallInst::toLL(llvm::Module *M, Context *K) {
  auto Callee = getCalleeFunction(Name, SourceLoc, M, K);
  auto RTy = cast<FunctionType>(cast<PointerType>(VTy)->getCTy())->getRTy();

  if (!getOperands().size()) {
    if (isa<VoidType>(RTy)) {
      K->Builder->CreateCall(Callee);
      return nullptr;
    }
    return K->Builder->CreateCall(Callee, Name);
  }

  // Prepare arguments to call
  std::vector<llvm::Value *> LLOps;
  for (auto Op: getOperands()) {
    LLOps.push_back(Op->toLL(M, K));
  }
  if (isa<VoidType>(RTy)) {
    K->Builder->CreateCall(Callee, LLOps);
    return nullptr;
  }
  return K->Builder->CreateCall(Callee, LLOps, Name);
}

llvm::Value *AddInst::toLL(llvm::Module *M, Context *K) {
  auto Op0 = getOperand(0)->toLL(M, K);
  auto Op1 = getOperand(1)->toLL(M, K);
  return K->Builder->CreateAdd(Op0, Op1);
}

llvm::Value *BindInst::toLL(llvm::Module *M, Context *K) {
  auto V = getVal()->toLL(M, K);
  auto Ty = getVal()->getType()->toLL(M, K);
  auto Name = getName();
  auto Alloca = K->Builder->CreateAlloca(Ty, nullptr, Name + "Alloca");
  K->Builder->CreateStore(V, Alloca);
  K->addMapping(Name, nullptr, Alloca);
  return nullptr;
}
}
