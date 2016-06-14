#include <fstream>
#include <sstream>
#include <iomanip>

#include "rhine/Diagnostic/Diagnostic.hpp"
#include "rhine/Parse/ParseDriver.hpp"
#include "rhine/Parse/Parser.hpp"
#include "rhine/Parse/Lexer.hpp"
#include "rhine/IR/Module.hpp"
#include "rhine/IR/Context.hpp"

namespace rhine {
class Module;

ParseDriver::ParseDriver(Module *Tree, bool Debug) :
    TraceScanning(Debug), Root(Tree), Ctx(Tree->context())
{}

bool ParseDriver::parseStream(std::istream &In,
                              const std::string &StreamName) {
  InputName = StreamName;
  rhine::Lexer Lex(In, Ctx->DiagPrinter->ErrorStream, this);
  Lex.set_debug(TraceScanning);
  Lexx = &Lex;

  Parser Parseyy(this);
  return Parseyy.parse();
}

bool ParseDriver::parseFile(const std::string &Filename) {
  std::ifstream in(Filename, std::ifstream::in);
  if (!in.good()) return false;
  return parseStream(in, Filename);
}

bool ParseDriver::parseString(const std::string &Input,
                              const std::string &StreamName) {
  StringStreamInput = &Input;
  std::istringstream Iss(Input);
  return parseStream(Iss, StreamName);
}
}
