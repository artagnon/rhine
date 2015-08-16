// -*- C++ -*-
#ifndef PARSEFACADE_H
#define PARSEFACADE_H

#include <string>
#include <iostream>

#include "llvm/IR/Module.h"
#include "rhine/IR/Module.h"
#include "rhine/Pass/ModulePass.h"

typedef int (*MainFTy)();

namespace rhine {
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
  llvm::Module *M;
  std::ostream &ErrStream;
  bool Debug;
public:
  ParseFacade(std::string &PrgString, std::ostream &ErrStream = std::cerr,
              llvm::Module *M = nullptr, bool Debug = false):
      PrgString(PrgString), M(M), ErrStream(ErrStream), Debug(Debug) {}

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
