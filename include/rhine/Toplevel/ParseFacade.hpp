// -*- C++ -*-
#ifndef PARSEFACADE_H
#define PARSEFACADE_H

#include <string>
#include <vector>
#include <iostream>

#include "rhine/Parse/ParseDriver.hpp"
#include "rhine/IR/Module.hpp"
#include "rhine/IR/Context.hpp"

typedef int (*MainFTy)();

namespace llvm {
class ExecutionEngine;
class Module;
}

namespace rhine {
class ModulePass;

enum class ParseSource { STRING, FILE };

enum class PostParseAction {
  IRString, /// Return, as a string, the pretty-printed Rhine IR
  LLString, /// Return, as a string, the pretty-printed LLVM IR
  LLEmit, /// Dump the constructed LLVM Module to stdout
  BCString, /// Return, as a string, the LLVM module converted to Bitcode
  BCWrite, /// Write a bitcode file from the LLVM module.
  LinkExecutable, /// Write bitcode, and call the system linker.
};

class ParseFacade {
  std::unique_ptr<llvm::Module> UniqueModule;
  std::unique_ptr<llvm::ExecutionEngine> EE;
  std::string PrgString;
  std::ostream &ErrStream;
  bool Debug;

public:
  /// ParseFacade requires the program string (or filename, indicated by
  /// ParseSource in different functions), an optional error stream, a debugging
  /// flag can also be set.
  ParseFacade(std::string PrgString, std::ostream &ErrStream = std::cerr,
              bool Debug = false);

  ParseFacade(const char *PrgString, std::ostream &ErrStream = std::cerr,
              bool Debug = false);

  /// Destroys Engines and Modules.
  ~ParseFacade();

  /// Quick helper that calls the pretty-print method on an IR object (Value or
  /// Module) and returns it as a string; useful mainly for testing.
  template <typename T> std::string irToPP(T *Obj);

  /// Quick helper that calls the pretty-print method on an LLVM IR object, and
  /// returns it as a string; mainly for testing.
  template <typename T> std::string llToPP(T *Obj);

  /// A little helper that factors out the job of writing a BitCode stream to a
  /// file on the disk. Used by both BCWrite and LinkExecutable.
  void writeBitcodeToFile();

  /// The main worker that takes the program source, parses it into Rhine IR,
  /// runs it through a series of transforms, and returns it, ready for
  /// conversion to LLVM IR.
  template<typename ...Ts>
  Module *parseToIR(ParseSource SrcE) {
    auto Ctx = llvm::make_unique<rhine::Context>(ErrStream);
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
    [](...){ }((Ts().runOnModule(Root), 0)...);
    return Root;
  }

  /// Helper that calls parseToIR with all the transforms.
  Module *parseToIR(ParseSource SrcE);

  /// Returns a string in ParseAction = LL and IR cases.
  std::string parseAction(ParseSource SrcE, PostParseAction ActionE);

  /// In addition to parseAction, run the IR through a JIT.
  MainFTy jitAction(ParseSource SrcE, PostParseAction ActionE);
};
}

#endif
