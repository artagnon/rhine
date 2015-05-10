//-*- C++ -*-

#ifndef PARSEDRIVER_H
#define PARSEDRIVER_H

#include <iostream>
#include <string>
#include <vector>
#include "rhine/ParseTree.h"
#include "rhine/Diagnostic.h"
#include "rhine/Context.h"

namespace rhine {

using namespace rhine;

class ParseDriver
{
public:

  /// construct a new parser driver context
  ParseDriver(class PTree &Tree, Context *SharedCtx,
              std::ostream &ErrStream = std::cerr,
              bool Debug = false) :
      TraceScanning(Debug), TraceParsing(Debug),
      ErrHandler(new DiagnosticPrinter(&ErrStream)),
      ErrorStream(&ErrStream), Root(Tree), Ctx(SharedCtx)
  {}

  bool parseStream(std::istream &In,
                   const std::string &StreamName = "stream input");
  bool parseString(const std::string &Input,
                   const std::string &StreamName = "string stream");
  bool parseFile(const std::string &filename);

  bool TraceScanning;
  bool TraceParsing;
  DiagnosticPrinter *ErrHandler;
  std::string StringStreamInput;
  std::string StreamName;
  std::ostream *ErrorStream;
  class Lexer *Lexx;
  class PTree &Root;
  Context *Ctx;
};
}

#endif
