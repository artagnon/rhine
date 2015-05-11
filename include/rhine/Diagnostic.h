//-*- C++ -*-

#ifndef DIAGNOSTIC_H
#define DIAGNOSTIC_H

#include <string>

namespace rhine {
struct DiagnosticPrinter {
  std::ostream *ErrorStream;
  std::string StringStreamInput;

  DiagnosticPrinter(std::ostream *ErrStream) : ErrorStream(ErrStream) {}
  void errorReport(const class location &Location,
                   const std::string &Message);
};
}


#endif
