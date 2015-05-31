#include "rhine/Toplevel/Toplevel.h"
#include "rhine/Toplevel/ParseFacade.h"
#include "rhine/Parse/ParseDriver.h"
#include "rhine/Externals.h"
#include "rhine/Support.h"

#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/Bitcode/BitstreamWriter.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/MCJIT.h"
#include "llvm/ExecutionEngine/SectionMemoryManager.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/Host.h"

#include "llvm/CodeGen/GCMetadataPrinter.h"
#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/Target/TargetLoweringObjectFile.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetSubtargetInfo.h"

#include <iostream>
#include <sstream>

using namespace llvm;

namespace rhine {
MainFTy jitFacade(std::string InStr, bool Debug, bool IsStringStream) {
  LLVMInitializeNativeTarget();
  LLVMInitializeNativeAsmPrinter();

  auto Owner = make_unique<llvm::Module>("main", llvm::getGlobalContext());
  auto Pf = ParseFacade(InStr, Owner.get(), std::cerr, Debug);
  if (IsStringStream)
    Pf.parseAction(ParseSource::STRING, PostParseAction::LL);
  else
    Pf.parseAction(ParseSource::FILE, PostParseAction::LLDUMP);
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
