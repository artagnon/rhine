#include <iostream>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cassert>

#include "rhine/Parse/Parser.h"
#include "rhine/Diagnostic/Diagnostic.h"

using Location = rhine::Parser::Location;

namespace rhine {
#define ANSI_COLOR_RED     "\x1b[31;1m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33;1m"
#define ANSI_COLOR_BLUE    "\x1b[34;1m"
#define ANSI_COLOR_MAGENTA "\x1b[35;1m"
#define ANSI_COLOR_CYAN    "\x1b[36;1m"
#define ANSI_COLOR_WHITE   "\x1b[37;1m"
#define ANSI_COLOR_RESET   "\x1b[0m"

class ColorCode
{
  std::string ColorF;
public:
  ColorCode(std::string Color) : ColorF(Color) {}
  friend std::ostream& operator<<(std::ostream& Dest, ColorCode const& Code)
  {
    if ((&Dest == &std::cerr && isatty(fileno(stderr))) ||
        (&Dest == &std::cout && isatty(fileno(stdout))))
      for (char ch : Code.ColorF) {
        Dest.put(ch);
      }
    return Dest;
  }
};

DiagnosticPrinter::DiagnosticPrinter(std::ostream *ErrStream) :
    ErrorStream(ErrStream) {}

void DiagnosticPrinter::errorReport(const Location &Loc,
                                    const std::string &Message) {
  assert(Loc.Begin.Filename == Loc.End.Filename);
  assert(Loc.Begin.Line == Loc.End.Line);

  std::istream *InStream;
  std::string ScriptPath;
  std::istringstream Iss(StringStreamInput);
  std::ifstream InFile(Loc.Begin.Filename);

  if (!StringStreamInput.empty()) {
    InStream = &Iss;
  } else {
    if (!InFile)
      *ErrorStream << ColorCode(ANSI_COLOR_RED) << "fatal: "
                   << ColorCode(ANSI_COLOR_WHITE)
                   << "Unable to open file" << Loc.Begin.Filename
                   << ColorCode(ANSI_COLOR_RESET) << std::endl;
    InStream = &InFile;
  }
  *ErrorStream << ColorCode(ANSI_COLOR_WHITE) << Loc.Begin.Filename << ":"
               << Loc.Begin.Line << ":" << Loc.Begin.Column << ": "
               << ColorCode(ANSI_COLOR_RED) << "error: "
               << ColorCode(ANSI_COLOR_WHITE) << Message
               << ColorCode(ANSI_COLOR_RESET) << std::endl;

  std::string FaultyLine;
  for (unsigned int i = 0; i < Loc.Begin.Line; i++)
    std::getline(*InStream, FaultyLine);

  *ErrorStream << FaultyLine << std::endl << std::setfill(' ')
               << std::setw(Loc.Begin.Column)
               << ColorCode(ANSI_COLOR_GREEN) << '^';
  unsigned int end_col = Loc.End.Column > 0 ? Loc.End.Column - 1 : 0;
  if (end_col != Loc.Begin.Column)
    *ErrorStream << std::setfill('~')
                 << std::setw(end_col - Loc.Begin.Column) << '~';
  *ErrorStream << ColorCode(ANSI_COLOR_RESET) << std::endl;
}
}
