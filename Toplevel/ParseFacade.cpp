#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Module.h"

#include "rhine/IR.h"
#include "rhine/ParseDriver.h"

#include <iostream>
#include <string>

namespace rhine {
std::string irToPP(Value *Obj)
{
  std::string Output;
  std::ostringstream OutputStream(Output);
  OutputStream << *Obj;
  return OutputStream.str();
}

std::string irToPP(Module *M)
{
  std::string Output;
  std::ostringstream OutputStream(Output);
  OutputStream << *M;
  return OutputStream.str();
}

std::string llToPP(llvm::Value *Obj)
{
  std::string Output;
  llvm::raw_string_ostream OutputStream(Output);
  Obj->print(OutputStream);
  return OutputStream.str();
}

std::string llToPP(llvm::Module *M)
{
  std::string Output;
  llvm::raw_string_ostream OutputStream(Output);
  M->print(OutputStream, nullptr);
  return OutputStream.str();
}

std::string parseTransformIR(std::string PrgString,
                             std::ostream &ErrStream,
                             bool Debug)
{
  auto Root = rhine::PTree();
  auto Ctx = rhine::Context();
  auto Driver = rhine::ParseDriver(Root, &Ctx, ErrStream, Debug);
  Driver.parseString(PrgString);
  Root.M.typeInfer(&Ctx);
  auto Ret = irToPP(&Root.M);
  Ctx.releaseMemory();
  return Ret;
}

std::string parseCodeGenString(std::string PrgString,
                               llvm::Module *M,
                               std::ostream &ErrStream,
                               bool Debug)
{
  auto Root = rhine::PTree();
  auto Ctx = rhine::Context();
  auto Driver = rhine::ParseDriver(Root, &Ctx, ErrStream, Debug);
  Driver.parseString(PrgString);
  Root.M.typeInfer(&Ctx);
  Root.M.toLL(M, &Ctx);
  Ctx.releaseMemory();
  return llToPP(M);
}

std::string parseCodeGenString(std::string PrgString,
                               std::ostream &ErrStream,
                               bool Debug)
{
  auto M = new llvm::Module("main", RhContext);
  return parseCodeGenString(PrgString, M, ErrStream, Debug);
}

void parseCodeGenFile(std::string Filename, llvm::Module *M, bool Debug) {
  auto Root = rhine::PTree();
  auto Ctx = rhine::Context();
  auto Driver = rhine::ParseDriver(Root, &Ctx, std::cerr, Debug);
  assert(Driver.parseFile(Filename) && "Could not parse file");
  Root.M.typeInfer(&Ctx);
  Root.M.toLL(M, &Ctx);
  Ctx.releaseMemory();
  M->dump();
}
}
