#include "rhine/Toplevel/ParseFacade.h"
#include "rhine/Parse/ParseDriver.h"
#include "rhine/Transform/LambdaLifting.h"
#include "rhine/Transform/TypeCoercion.h"
#include "rhine/IR.h"

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

std::string ParseFacade::parseAction(ParseSource SrcE,
                                     PostParseAction ActionE)
{
  std::string Ret;
  rhine::PTree Root;
  rhine::Context Ctx(ErrStream);
  auto Driver = rhine::ParseDriver(Root, &Ctx, Debug);
  switch(SrcE) {
  case ParseSource::STRING:
    if (!Driver.parseString(PrgString))
      exit(1);
    break;
  case ParseSource::FILE:
    if (!Driver.parseFile(PrgString))
      exit(1);
    break;
  }
  auto LambLift = LambdaLifting(&Ctx);
  LambLift.runOnModule(&Root.M);
  Root.M.typeInfer(&Ctx);
  auto TyCoerce = TypeCoercion(&Ctx);
  TyCoerce.runOnModule(&Root.M);
  switch(ActionE) {
  case PostParseAction::IR:
    Ret = irToPP(&Root.M);
    break;
  case PostParseAction::LL:
    Root.M.toLL(M, &Ctx);
    Ret = llToPP(M);
    break;
  case PostParseAction::LLDUMP:
    Root.M.toLL(M, &Ctx);
    M->dump();
    break;
  }
  Ctx.releaseMemory();
  return Ret;
}

std::string parseTransformIR(std::string PrgString,
                             std::ostream &ErrStream,
                             bool Debug)
{
  auto Pf = ParseFacade(PrgString, nullptr, ErrStream, Debug);
  return Pf.parseAction(ParseSource::STRING, PostParseAction::IR);
}
std::string parseCodeGenString(std::string PrgString,
                               llvm::Module *M,
                               std::ostream &ErrStream,
                               bool Debug)
{
  auto Pf = ParseFacade(PrgString, M, ErrStream, Debug);
  return Pf.parseAction(ParseSource::STRING, PostParseAction::LL);
}

std::string parseCodeGenString(std::string PrgString,
                               std::ostream &ErrStream,
                               bool Debug)
{
  auto M = new llvm::Module("main", llvm::getGlobalContext());
  return parseCodeGenString(PrgString, M, ErrStream, Debug);
}

void parseCodeGenFile(std::string Filename, llvm::Module *M, bool Debug) {
  auto Pf = ParseFacade(Filename, M, std::cerr, Debug);
  Pf.parseAction(ParseSource::FILE, PostParseAction::LLDUMP);
}
}
