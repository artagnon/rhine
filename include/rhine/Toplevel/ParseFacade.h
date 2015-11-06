// -*- C++ -*-
#ifndef PARSEFACADE_H
#define PARSEFACADE_H

#include <string>
#include <vector>
#include <iostream>

typedef int (*MainFTy)();

namespace llvm {
class ExecutionEngine;
class Module;
}

namespace rhine {
class ModulePass;
class Module;

enum class ParseSource { STRING, FILE };

enum class PostParseAction {
  IR, /// Return, as a string, the pretty-printed Rhine IR
  LL, /// Return, as a string, the pretty-printed LLVM IR
  LLDUMP, /// Dump the constructed LLVM Module to stdout
};

class ParseFacade {
  std::vector<llvm::ExecutionEngine *> Engines;
  std::unique_ptr<llvm::Module> UniqueModule;
  std::string &PrgString;
  std::ostream &ErrStream;
  bool Debug;

public:
  /// ParseFacade requires the program string (or filename, indicated by
  /// ParseSource in different functions), an optional error stream, a debugging
  /// flag can also be set.
  ParseFacade(std::string &PrgString, std::ostream &ErrStream = std::cerr,
              bool Debug = false);

  /// Destroys Engines and Modules.
  ~ParseFacade();

  /// Quick helper that calls the pretty-print method on an IR object (Value or
  /// Module) and returns it as a string; useful mainly for testing.
  template <typename T> std::string irToPP(T *Obj);

  /// Quick helper that calls the pretty-print method on an LLVM IR object, and
  /// returns it as a string; mainly for testing.
  template <typename T> std::string llToPP(T *Obj);

  /// The main worker that takes the program source, parses it into Rhine IR,
  /// runs it through a series of transforms, and returns it, ready for
  /// conversion to LLVM IR.
  Module *parseToIR(ParseSource SrcE, std::vector<ModulePass *> TransformChain);

  /// Returns a string in ParseAction = LL and IR cases.
  std::string parseAction(ParseSource SrcE, PostParseAction ActionE);

  /// In addition to parseAction, run the IR through a JIT.
  MainFTy jitAction(ParseSource SrcE, PostParseAction ActionE);
};
}

#endif
