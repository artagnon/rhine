#include "rhine/Toplevel/ParseFacade.h"
#include "rhine/Parse/ParseDriver.h"
#include "rhine/Transform/TypeInfer.h"
#include "rhine/Transform/LambdaLifting.h"
#include "rhine/Transform/TypeCoercion.h"
#include "rhine/Transform/ResolveLocals.h"
#include "rhine/Runtime/GC.h"
#include "rhine/IR.h"

#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Module.h"

#include <iostream>
#include <string>

namespace rhine {
template <typename T>
std::string ParseFacade::irToPP(T *Obj)
{
  std::string Output;
  std::ostringstream OutputStream(Output);
  OutputStream << *Obj;
  return OutputStream.str();
}

template <typename T>
std::string ParseFacade::llToPP(T *Obj)
{
  std::string Output;
  llvm::raw_string_ostream OutputStream(Output);
  Obj->print(OutputStream, nullptr);
  return OutputStream.str();
}

Module *ParseFacade::parseToIR(Context *Ctx, ParseSource SrcE) {
  auto Root = new rhine::PTree(Ctx);
  auto Driver = rhine::ParseDriver(*Root, Ctx, Debug);
  switch(SrcE) {
  case ParseSource::STRING:
    if (!Driver.parseString(PrgString)) {
      std::cerr << "Could not parse string" << std::endl;
      exit(1);
    }
    break;
  case ParseSource::FILE:
    if (!Driver.parseFile(PrgString)) {
      std::cerr << "Could not parse file" << std::endl;
      exit(1);
    }
    break;
  }
  std::vector<ModulePass *> TransformChain =
    { new ResolveLocals, new LambdaLifting,
      new TypeInfer, new TypeCoercion };
  for (auto Transform : TransformChain) {
    Transform->runOnModule(Root->M);
    delete Transform;
  }
  auto M = Root->M;
  delete Root;
  return M;
}

std::string ParseFacade::parseAction(ParseSource SrcE,
                                     PostParseAction ActionE)
{
  std::string Ret;
  auto Ctx = new rhine::Context(ErrStream);
  auto R = parseToIR(Ctx, SrcE);
  switch(ActionE) {
  case PostParseAction::IR:
    Ret = irToPP(R);
    break;
  case PostParseAction::LL:
    if (!M)
      M = new llvm::Module("main", Ctx->LLContext);
    R->toLL(M);
    Ret = llToPP(M);
    break;
  case PostParseAction::LLDUMP:
    if (!M)
      M = new llvm::Module("main", Ctx->LLContext);
    R->toLL(M);
    M->dump();
    break;
  }
  for (auto F : *R) {
    for (auto V : *F) {
      delete V;
    }
    delete F;
  }
  Ctx->releaseMemory();
  delete Ctx;
  return Ret;
}

MainFTy ParseFacade::jitAction(ParseSource SrcE, PostParseAction ActionE) {
  LLVMInitializeNativeTarget();
  LLVMInitializeNativeAsmPrinter();

  auto Owner = make_unique<llvm::Module>("main", llvm::getGlobalContext());
  M = Owner.get();
  parseAction(SrcE, ActionE);
  auto EE = EngineBuilder(std::move(Owner)).create();
  assert(EE && "Error creating MCJIT with EngineBuilder");
  union {
    uint64_t raw;
    MainFTy usable;
  } functionPointer;
  functionPointer.raw = EE->getFunctionAddress("main");
  assert(functionPointer.usable && "no main function found");
  return functionPointer.usable;
}
}
