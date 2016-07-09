#include "rhine/Toplevel/ParseFacade.hpp"
#include "rhine/IR/Context.hpp"
#include "rhine/IR/Module.hpp"
#include "rhine/Parse/ParseDriver.hpp"
#include "rhine/Transform/LambdaLifting.hpp"
#include "rhine/Transform/Resolve.hpp"
#include "rhine/Transform/Scope2Block.hpp"
#include "rhine/Transform/TypeCoercion.hpp"
#include "rhine/Transform/TypeInfer.hpp"

#include "llvm/Bitcode/ReaderWriter.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/FileSystem.h"

#include <cstdlib>
#include <iostream>
#include <string>

namespace rhine {
ParseFacade::ParseFacade(std::string PrgString, std::ostream &ErrStream,
                         bool Debug)
    : PrgString(PrgString), ErrStream(ErrStream), Debug(Debug) {}

ParseFacade::ParseFacade(const char *PrgString, std::ostream &ErrStream,
                         bool Debug)
    : PrgString(PrgString), ErrStream(ErrStream), Debug(Debug) {}

ParseFacade::~ParseFacade() {}

Module *ParseFacade::parseToIR(ParseSource SrcE) {
  return parseToIR<Scope2Block, LambdaLifting, Resolve, TypeInfer,
                   TypeCoercion>(SrcE);
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
  auto TransformedIR = std::unique_ptr<Module>(parseToIR(SrcE));
  UniqueModule.reset(new llvm::Module("main", TransformedIR->llvmContext()));
  auto RawModule = UniqueModule.get();
  if (ActionE != PostParseAction::IRString)
    TransformedIR->generate(RawModule);
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
    if (auto AssemblerProcess = popen("llc foo.bc", "r")) {
      if (auto ExitStatus = pclose(AssemblerProcess) / 256) {
        std::cerr << "Assembler exited with nonzero status: " << ExitStatus;
        exit(1);
      }
    } else {
      std::cerr << "llc not found" << std::endl;
      exit(1);
    }
    if (auto LinkerProcess = popen("clang -o foo foo.s", "r")) {
      if (auto ExitStatus = pclose(LinkerProcess) / 256) {
        std::cerr << "Linker exited with nonzero status: " << ExitStatus;
        exit(1);
      }
      break;
    }
    std::cerr << "clang not found" << std::endl;
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
