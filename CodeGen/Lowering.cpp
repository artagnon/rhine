#include "rhine/IR.h"
#include "rhine/Context.h"
#include "rhine/Externals.h"

namespace rhine {
llvm::Type *IntegerType::toLL(llvm::Module *M, Context *K) {
  return RhBuilder.getInt32Ty();
}

llvm::Type *BoolType::toLL(llvm::Module *M, Context *K) {
  return RhBuilder.getInt1Ty();
}

llvm::Type *FloatType::toLL(llvm::Module *M, Context *K) {
  return RhBuilder.getFloatTy();
}

llvm::Type *StringType::toLL(llvm::Module *M, Context *K) {
  return RhBuilder.getInt8PtrTy();
}

llvm::Type *FunctionType::toLL(llvm::Module *M, Context *K) {
  assert(0 && "not yet implemented");
}

llvm::Value *Symbol::toLL(llvm::Module *M, Context *K) {
  assert(K && "null Symbol Table");
  return K->getMapping(getName(), getSourceLocation());
}

llvm::Value *GlobalString::toLL(llvm::Module *M, Context *K) {
  auto SRef = llvm::StringRef(getVal());
  return RhBuilder.CreateGlobalStringPtr(SRef);
}

llvm::Constant *ConstantInt::toLL(llvm::Module *M, Context *K) {
  return llvm::ConstantInt::get(RhContext, APInt(32, getVal()));
}

llvm::Constant *ConstantBool::toLL(llvm::Module *M, Context *K) {
  return llvm::ConstantInt::get(RhContext, APInt(1, getVal()));
}

llvm::Constant *ConstantFloat::toLL(llvm::Module *M, Context *K) {
  return llvm::ConstantFP::get(RhContext, APFloat(getVal()));
}

llvm::Constant *Function::toLL(llvm::Module *M, Context *K) {
  auto FType = dyn_cast<FunctionType>(getType());
  auto RType = FType->getRTy()->toLL(M, K);
  std::vector<llvm::Type *> ArgTys;
  for (auto RhTy: FType->getATys())
    ArgTys.push_back(RhTy->toLL());
  auto ArgTyAr = makeArrayRef(ArgTys);
  auto F = llvm::Function::Create(llvm::FunctionType::get(RType, ArgTyAr, false),
                                  GlobalValue::ExternalLinkage,
                                  getName(), M);

  // Bind argument symbols to function argument values in symbol table
  auto ArgList = getArgumentList();
  auto S = ArgList.begin();
  auto V = F->arg_begin();
  auto SEnd = ArgList.end();
  auto VEnd = F->arg_end();
  for (; S != SEnd && V != VEnd; ++S, ++V)
    K->addMapping((*S)->getName(), V);

  // Add function symbol to symbol table
  K->addMapping(getName(), F);

  BasicBlock *BB = BasicBlock::Create(rhine::RhContext, "entry", F);
  RhBuilder.SetInsertPoint(BB);
  llvm::Value *LastLL;
  for (auto Val : getVal())
    LastLL = Val->toLL(M, K);
  RhBuilder.CreateRet(LastLL);
  return F;
}

llvm::Value *AddInst::toLL(llvm::Module *M, Context *K) {
  auto Op0 = getOperand(0)->toLL();
  auto Op1 = getOperand(1)->toLL();
  return RhBuilder.CreateAdd(Op0, Op1);
}

llvm::Value *CallInst::toLL(llvm::Module *M, Context *K) {
  location SourceLoc = getSourceLocation();
  llvm::Function *Callee;
  auto Name = getName();
  if (auto Result = K->getMapping(Name)) {
    if (auto CalleeCandidate = dyn_cast<llvm::Function>(Result))
      Callee = CalleeCandidate;
    else {
      K->DiagPrinter->errorReport(
          SourceLoc, Name + " was not declared as a function");
      exit(1);
    }
  } else if (auto FPtr = Externals::getMapping(Name)) {
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

  auto Arg = getOperand(0);
  llvm::Value *ArgLL;
  if (auto Sym = dyn_cast<Symbol>(Arg))
    ArgLL = K->getMapping(Sym->getName(), Sym->getSourceLocation());
  else
    ArgLL = Arg->toLL(M);

  return RhBuilder.CreateCall(Callee, ArgLL, getName());
}

llvm::Value *BindInst::toLL(llvm::Module *M, Context *K) {
  auto V = getVal()->toLL();
  K->addMapping(getName(), V);
  return nullptr;
}

void Module::toLL(llvm::Module *M, Context *K) {
  for (auto F: getVal())
    F->toLL(M, K);
}
}
