// -*- C++ -*-
#ifndef PARSEFACADE_H
#define PARSEFACADE_H

#include <string>
#include <iostream>

#include "llvm/IR/Module.h"

typedef int (*MainFTy)();

namespace rhine {
class ModulePass;
class Module;

enum class ParseSource {
  STRING,
  FILE
};

enum class PostParseAction {
  IR,
  LL,
  LLDUMP,
};

class ParseFacade {
  std::string &PrgString;
  llvm::Module *ProgramModule;
  std::ostream &ErrStream;
  bool Debug;
public:
  ParseFacade(std::string &PrgString, std::ostream &ErrStream = std::cerr,
              llvm::Module *ProgramModule = nullptr, bool Debug = false):
      PrgString(PrgString), ProgramModule(ProgramModule),
      ErrStream(ErrStream), Debug(Debug) {}

  template <typename T>
  std::string irToPP(T *Obj);

  template <typename T>
  std::string llToPP(T *Obj);

  Module *parseToIR(ParseSource SrcE, std::vector<ModulePass *> TransformChain);
  std::string parseAction(ParseSource SrcE, PostParseAction ActionE);
  MainFTy jitAction(ParseSource SrcE, PostParseAction ActionE);
};
}

#endif
