#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Module.h"

#include "rhine/IR.h"
#include "rhine/ParseDriver.h"

#include <iostream>
#include <string>

namespace rhine {
std::string llToPP (llvm::Value *Obj)
{
  std::string Output;
  llvm::raw_string_ostream OutputStream(Output);
  Obj->print(OutputStream);
  return OutputStream.str();
}

std::string llToPP (llvm::Module *M)
{
  std::string Output;
  llvm::raw_string_ostream OutputStream(Output);
  M->print(OutputStream, nullptr);
  return OutputStream.str();
}

std::string parseCodeGenString(std::string PrgString,
                               llvm::Module *M,
                               std::ostream &ErrStream,
                               bool Debug)
{
  auto Root = rhine::PTree();
  auto Driver = rhine::ParseDriver(Root, ErrStream, Debug);
  Driver.parseString(PrgString);
  Root.M.toLL(M);
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
  auto Driver = rhine::ParseDriver(Root, std::cerr, Debug);
  assert(Driver.parseFile(Filename) && "Could not parse file");
  Root.M.toLL(M);
  M->dump();
}
}
