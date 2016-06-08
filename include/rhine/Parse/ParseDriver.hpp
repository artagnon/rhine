#pragma once

#include <iostream>
#include <string>
#include <vector>

namespace rhine {
class Context;
class Module;
class Lexer;

class ParseDriver {
public:
  /// We fill in the parsetree and hand it back; Tree is initialized with a
  /// Context that we borrow. Debug is useful for getting trace outputs
  ParseDriver(Module *Tree, bool Debug = false);

  /// The main driver that is called by parseString and parseFile; sets things
  /// up (instatiates the lexer), and calls the parser
  bool parseStream(std::istream &In, const std::string &StreamName_);

  /// For things like unittests
  bool parseString(const std::string &Input,
                   const std::string &StreamName_ = "string stream");

  /// For real files with filenames
  bool parseFile(const std::string &Filename);

  /// Name of current stream being parsed; used in filling in location
  /// information into the AST by the parser
  std::string InputName;

  /// Actual string input (nullptr in the case of file)
  const std::string *StringStreamInput = nullptr;

  /// The lexer, used as a service by the parser
  Lexer *Lexx;

  /// Give verbose Flex output
  bool TraceScanning;

  /// The Root of the ParseTree into which the parser stuffs whatever
  Module *Root;

  /// Finally, the Context
  Context *Ctx;
};
}
