#include "rhine/Toplevel/ParseFacade.h"
#include "rhine/Parse/ParseDriver.h"
#include "rhine/Parse/ParseTree.h"
#include "rhine/Transform/FlattenBB.h"
#include "rhine/Transform/TypeInfer.h"
#include "rhine/Transform/LambdaLifting.h"
#include "rhine/Transform/TypeCoercion.h"
#include "rhine/Transform/Resolve.h"
#include "rhine/Runtime/GC.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Module.h"

#include <iostream>
#include <string>

namespace rhine {
template <typename T> std::string ParseFacade::irToPP(T *Obj) {
  std::string Output;
  std::ostringstream OutputStream(Output);
  OutputStream << *Obj;
  return OutputStream.str();
}

template <typename T> std::string ParseFacade::llToPP(T *Obj) {
  std::string Output;
  llvm::raw_string_ostream OutputStream(Output);
  Obj->print(OutputStream, nullptr);
  return OutputStream.str();
}

Module *ParseFacade::parseToIR(ParseSource SrcE,
                               std::vector<ModulePass *> TransformChain) {
  auto Ctx = new rhine::Context(ErrStream);
  rhine::PTree Root(Ctx);
  auto Driver = rhine::ParseDriver(Root, Ctx, Debug);
  switch (SrcE) {
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
  auto RootModule = Root.M;
  for (auto Transform : TransformChain)
    Transform->runOnModule(RootModule);
  return RootModule;
}

std::string ParseFacade::parseAction(ParseSource SrcE,
                                     PostParseAction ActionE) {
  Resolve ResolveL;
  LambdaLifting LambLift;
  FlattenBB Flatten;
  TypeInfer TyInfer;
  TypeCoercion TyCoercion;
  auto TransformedIR =
      parseToIR(SrcE, {&LambLift, &Flatten, &ResolveL, &TyInfer, &TyCoercion});
  std::string Ret;
  auto Ctx = TransformedIR->getContext();
  switch (ActionE) {
  case PostParseAction::IR:
    Ret = irToPP(TransformedIR);
    break;
  case PostParseAction::LL:
    if (!ProgramModule)
      ProgramModule = new llvm::Module("main", Ctx->LLContext);
    TransformedIR->toLL(ProgramModule);
    Ret = llToPP(ProgramModule);
    break;
  case PostParseAction::LLDUMP:
    if (!ProgramModule)
      ProgramModule = new llvm::Module("main", Ctx->LLContext);
    TransformedIR->toLL(ProgramModule);
    ProgramModule->dump();
    break;
  }
  delete TransformedIR;
  Ctx->releaseMemory();
  delete Ctx;
  return Ret;
}

MainFTy ParseFacade::jitAction(ParseSource SrcE, PostParseAction ActionE) {
  LLVMInitializeNativeTarget();
  LLVMInitializeNativeAsmPrinter();

  auto UniqueModule =
      make_unique<llvm::Module>("main", llvm::getGlobalContext());
  ProgramModule = UniqueModule.get();
  parseAction(SrcE, ActionE);

  std::string ErrorStr;
  auto EE = EngineBuilder(std::move(UniqueModule))
                .setEngineKind(llvm::EngineKind::Either)
                .setErrorStr(&ErrorStr)
                .create();
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
