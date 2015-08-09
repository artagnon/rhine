//-*- C++ -*-

#ifndef PARSEDRIVER_H
#define PARSEDRIVER_H

#include <iostream>
#include <string>
#include <vector>
#include "rhine/Parse/ParseTree.h"
#include "rhine/Context.h"

namespace rhine {

using namespace rhine;

class ParseDriver
{
public:

  /// construct a new parser driver context
  ParseDriver(class PTree &Tree, Context *SharedCtx,
              bool Debug = false) :
      TraceScanning(Debug), TraceParsing(Debug),
      Root(Tree), Ctx(SharedCtx)
  {}

  bool parseStream(std::istream &In,
                   const std::string &StreamName_ = "stream input");
  bool parseString(const std::string &Input,
                   const std::string &StreamName_ = "string stream");
  bool parseFile(const std::string &filename);

  bool TraceScanning;
  bool TraceParsing;
  std::string StreamName;
  class Lexer *Lexx;
  PTree &Root;
  Context *Ctx;
};
}

#endif
