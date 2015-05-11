//-*- C++ -*-

#ifndef DIAGNOSTIC_H
#define DIAGNOSTIC_H

#include <string>

namespace rhine {
class DiagnosticPrinter {
  std::string StringStreamInput;
public:
  std::ostream *ErrorStream;
  DiagnosticPrinter(std::ostream *ErrStream) : ErrorStream(ErrStream) {}
  void errorReport(const class location &Location,
                   const std::string &Message,
                   std::string StringStreamInput = "");
};
}


#endif
