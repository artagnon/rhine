#include "rhine/Toplevel/ParseFacade.h"
#include "rhine/Parse/ParseDriver.h"
#include "rhine/Transform/Scope2Block.h"
#include "rhine/Transform/TypeInfer.h"
#include "rhine/Transform/LambdaLifting.h"
#include "rhine/Transform/TypeCoercion.h"
#include "rhine/Transform/Resolve.h"
#include "rhine/IR/Context.h"
#include "rhine/IR/Module.h"

#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/Bitcode/ReaderWriter.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/IR/Module.h"

#include <iostream>
#include <cstdlib>
#include <string>

namespace rhine {
ParseFacade::ParseFacade(std::string PrgString, std::ostream &ErrStream,
                         bool Debug)
    : PrgString(PrgString), ErrStream(ErrStream), Debug(Debug) {}

  ParseFacade::ParseFacade(const char *PrgString, std::ostream &ErrStream,
                           bool Debug)
      : PrgString(std::string(PrgString)), ErrStream(ErrStream), Debug(Debug) {}

ParseFacade::~ParseFacade() {}

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
  auto Ctx = make_unique<rhine::Context>(ErrStream);
  auto Root = Module::get(std::move(Ctx));
  auto Driver = rhine::ParseDriver(Root, Debug);
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
  for (auto Transform : TransformChain)
    Transform->runOnModule(Root);
  return Root;
}

void ParseFacade::writeBitcodeToFile() {
  std::error_code EC;
  llvm::raw_fd_ostream OutputFile("foo.bc", EC, sys::fs::F_None);
  llvm::WriteBitcodeToFile(UniqueModule.get(), OutputFile);
  if (EC) {
    std::cerr << EC.message() << std::endl;
    exit(1);
  }
}

std::string ParseFacade::parseAction(ParseSource SrcE,
                                     PostParseAction ActionE) {
  Resolve ResolveL;
  LambdaLifting LambLift;
  Scope2Block Flatten;
  TypeInfer TyInfer;
  TypeCoercion TyCoercion;
  auto TransformedIR = std::unique_ptr<Module>(
      parseToIR(SrcE, {&LambLift, &Flatten, &ResolveL, &TyInfer, &TyCoercion}));
  UniqueModule.reset(new llvm::Module("main", llvm::getGlobalContext()));
  auto RawModule = UniqueModule.get();
  if (ActionE != PostParseAction::IRString)
    TransformedIR->toLL(RawModule);
  switch (ActionE) {
  case PostParseAction::IRString:
    return irToPP(TransformedIR.get());
  case PostParseAction::LLString:
    return llToPP(RawModule);
  case PostParseAction::LLEmit:
    UniqueModule->dump();
    break;
  case PostParseAction::BCString: {
    std::string Output;
    llvm::raw_string_ostream OutputStream(Output);
    llvm::WriteBitcodeToFile(RawModule, OutputStream);
    return OutputStream.str();
  }
  case PostParseAction::BCWrite:
    writeBitcodeToFile();
    break;
  case PostParseAction::LinkExecutable:
    writeBitcodeToFile();
    if (auto LinkerProcess = popen("clang -o foo foo.bc", "r")) {
      if (auto ExitStatus = pclose(LinkerProcess)/256) {
        std::cerr << "Linker exited with nonzero status: " << ExitStatus;
        exit(1);
      }
      break;
    }
    std::cerr << "Linker didn't start: " << strerror(errno);
    exit(1);
  }
  return "";
}

MainFTy ParseFacade::jitAction(ParseSource SrcE, PostParseAction ActionE) {
  LLVMInitializeNativeTarget();
  LLVMInitializeNativeAsmPrinter();

  parseAction(SrcE, ActionE);
  std::string ErrorStr;
  EE.reset(EngineBuilder(std::move(UniqueModule))
               .setEngineKind(llvm::EngineKind::Either)
               .setErrorStr(&ErrorStr)
               .create());
  union {
    uint64_t raw;
    MainFTy usable;
  } functionPointer;
  functionPointer.raw = EE->getFunctionAddress("main");
  assert(functionPointer.usable && "no main function found");
  return functionPointer.usable;
}
}
