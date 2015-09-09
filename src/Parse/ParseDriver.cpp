#include <fstream>
#include <sstream>
#include <iomanip>

#include "rhine/Parse/ParseDriver.h"
#include "rhine/Parse/Lexer.h"
#include "rhine/Context.h"

namespace rhine {
ParseDriver::ParseDriver(class PTree &Tree, Context *SharedCtx, bool Debug) :
    TraceScanning(Debug), TraceParsing(Debug), Root(Tree), Ctx(SharedCtx)
{}

bool ParseDriver::parseStream(std::istream &In,
                              const std::string &StreamName_) {
  Ctx->DiagPrinter->StreamName = StreamName_;
  rhine::Lexer Lex(&In, Ctx->DiagPrinter->ErrorStream, this);
  Lex.set_debug(TraceScanning);
  Lexx = &Lex;

  Parser Parseyy(this);
  // Parseyy.set_debug_level(TraceParsing);
  return !Parseyy.parse();
}

bool ParseDriver::parseFile(const std::string &Filename) {
  std::ifstream in(Filename, std::ifstream::in);
  if (!in.good()) return false;
  return parseStream(in, Filename);
}

bool ParseDriver::parseString(const std::string &Input,
                              const std::string &StreamName_) {
  Ctx->DiagPrinter->StringStreamInput = Input;
  std::istringstream Iss(Input);
  return parseStream(Iss, StreamName_);
}
}
