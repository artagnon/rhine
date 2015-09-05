#include <fstream>
#include <sstream>
#include <iomanip>
#include <cassert>
#include <unistd.h>

#include "location.hh"
#include "rhine/Diagnostic.h"

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

void DiagnosticPrinter::errorReport(const location &Location,
                                    const std::string &Message) {
  assert(Location.begin.filename == Location.end.filename);
  assert(Location.begin.line == Location.end.line);

  std::istream *InStream;
  std::string ScriptPath;
  std::istringstream Iss(StringStreamInput);
  std::ifstream InFile(*Location.begin.filename);

  if (!StringStreamInput.empty()) {
    InStream = &Iss;
  } else {
    if (!InFile)
      *ErrorStream << ColorCode(ANSI_COLOR_RED) << "fatal: "
                   << ColorCode(ANSI_COLOR_WHITE)
                   << "Unable to open file" << *Location.begin.filename
                   << ColorCode(ANSI_COLOR_RESET) << std::endl;
    InStream = &InFile;
  }
  *ErrorStream << ColorCode(ANSI_COLOR_WHITE) << *Location.begin.filename << ":"
               << Location.begin.line << ":" << Location.begin.column << ": "
               << ColorCode(ANSI_COLOR_RED) << "error: "
               << ColorCode(ANSI_COLOR_WHITE) << Message
               << ColorCode(ANSI_COLOR_RESET) << std::endl;

  std::string FaultyLine;
  for (unsigned int i = 0; i < Location.begin.line; i++)
    std::getline(*InStream, FaultyLine);

  *ErrorStream << FaultyLine << std::endl << std::setfill(' ')
               << std::setw(Location.begin.column)
               << ColorCode(ANSI_COLOR_GREEN) << '^';
  unsigned int end_col = Location.end.column > 0 ? Location.end.column - 1 : 0;
  if (end_col != Location.begin.column)
    *ErrorStream << std::setfill('~')
                 << std::setw(end_col - Location.begin.column) << '~';
  *ErrorStream << ColorCode(ANSI_COLOR_RESET) << std::endl;
}
}
