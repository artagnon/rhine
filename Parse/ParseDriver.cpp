// $Id$
/** \file driver.cc Implementation of the example::Driver class. */

#include <fstream>
#include <sstream>
#include <iomanip>
#include <unistd.h>

#include "rhine/ParseDriver.h"
#include "rhine/Lexer.h"
#include "Parser.hpp"

namespace rhine {
bool ParseDriver::parseStream(std::istream &In,
                              const std::string &StreamName) {
  this->StreamName = StreamName;

  rhine::Lexer Lexx(&In, ErrorStream, this);
  Lexx.set_debug(TraceScanning);
  this->Lexx = &Lexx;

  Parser Parseyy(this);
  Parseyy.set_debug_level(TraceParsing);
  return !Parseyy.parse();
}

bool ParseDriver::parseFile(const std::string &Filename) {
  std::ifstream in(Filename, std::ifstream::in);
  if (!in.good()) return false;
  return parseStream(in, Filename);
}

bool ParseDriver::parseString(const std::string &Input,
                              const std::string &StreamName)
{
  StringStreamInput = Input;
  std::istringstream Iss(Input);
  return parseStream(Iss, StreamName);
}
}
