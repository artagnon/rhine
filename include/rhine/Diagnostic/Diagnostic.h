//-*- C++ -*-

#ifndef DIAGNOSTIC_H
#define DIAGNOSTIC_H

#include <string>

#include "rhine/Parse/Parser.h"

namespace rhine {
class DiagnosticPrinter {
public:
  std::ostream *ErrorStream;
  std::string StringStreamInput;

  DiagnosticPrinter(std::ostream *ErrStream);
  void errorReport(const rhine::Parser::Location &Loca,
                   const std::string &Message);
};
}


#endif
