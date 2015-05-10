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
  // Necessary to prevent color codes from messing up setw
  friend std::ostream& operator<<(std::ostream& dest, ColorCode const& Code)
  {
    for (char ch : Code.ColorF) {
      dest.put(ch);
    }
    return dest;
  }
};

void DiagnosticPrinter::errorReport(const location &Location,
                                    const std::string &Message,
                                    std::string StringStreamInput) {
  // TODO: when are these assumptions violated?
  assert(Location.begin.filename == Location.end.filename);
  assert(Location.begin.line == Location.end.line);

  std::istream *InStream;
  std::string ScriptPath;
  std::istringstream Iss(StringStreamInput);
  std::ifstream InFile(*Location.begin.filename);

  if (!StringStreamInput.empty()) {
    InStream = &Iss;
    ScriptPath = *Location.begin.filename;
  } else {
    if (!InFile)
      *ErrorStream << ColorCode(ANSI_COLOR_RED) << "fatal: "
                   << ColorCode(ANSI_COLOR_WHITE)
                   << "Unable to open file" << *Location.begin.filename
                   << ColorCode(ANSI_COLOR_RESET) << std::endl;
    InStream = &InFile;
    ScriptPath = *Location.begin.filename;
    char Buffer[PATH_MAX];
    char *CwdP = getcwd(Buffer, PATH_MAX);
    std::string Cwd(CwdP);
    Cwd += "/"; // Cwd is guaranteed to be a directory
    auto MisPair = std::mismatch(Cwd.begin(),
                                 Cwd.end(), ScriptPath.begin());
    auto CommonAnchor = Cwd.rfind('/', MisPair.first - Cwd.begin());
    std::string StripString = "";
    if (MisPair.first != Cwd.end()) {
      auto StripComponents = std::count(MisPair.first, Cwd.end(), '/');
      for (int i = 0; i < StripComponents; i++)
        StripString += "../";
    }
    ScriptPath.erase(ScriptPath.begin(),
                     ScriptPath.begin() + CommonAnchor + 1);
    // +1 for the '/'
    ScriptPath = StripString + ScriptPath;
  }
  *ErrorStream << ColorCode(ANSI_COLOR_WHITE) << ScriptPath << ":"
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
