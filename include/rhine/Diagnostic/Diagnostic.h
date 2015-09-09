//-*- C++ -*-

#ifndef DIAGNOSTIC_H
#define DIAGNOSTIC_H

#include <string>

namespace rhine {
class DiagnosticPrinter {
public:
  std::string StreamName;
  std::ostream *ErrorStream;
  std::string StringStreamInput;

  DiagnosticPrinter(std::ostream *ErrStream);
  void errorReport(const class rhine::Parser::Location &Loca,
                   const std::string &Message);
};
}


#endif
