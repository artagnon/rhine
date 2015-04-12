//-*- C++ -*-

#ifndef PARSEDRIVER_H
#define PARSEDRIVER_H

#include <iostream>
#include <string>
#include <vector>
#include "rhine/ParseTree.h"
#include "rhine/Context.h"

namespace rhine {

using namespace rhine;

class ParseDriver
{
public:

  /// construct a new parser driver context
  ParseDriver(class PTree &Tree, Context *SharedCtx,
              std::ostream &ErrStream = std::cerr,
              bool Debug = false) : TraceScanning(Debug),
                                    TraceParsing(Debug),
                                    ErrorStream(&ErrStream),
                                    Root(Tree),
                                    Ctx(SharedCtx)
  {}

  bool parseStream(std::istream &in,
                   const std::string &sname = "stream input");
  bool parseString(const std::string &input,
                   const std::string &sname = "string stream");
  bool parseFile(const std::string &filename);
  void error(const class location &l, const std::string &m);
  void error(const std::string &m);

  bool TraceScanning;
  bool TraceParsing;
  std::string StringStreamInput;
  std::string StreamName;
  std::ostream *ErrorStream;
  class Lexer *Lexx;
  class PTree &Root;
  Context *Ctx;
};
}

#endif
