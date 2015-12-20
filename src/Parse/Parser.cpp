#include "rhine/Parse/ParseDriver.hpp"
#include "rhine/Parse/Parser.hpp"
#include "rhine/Parse/Lexer.hpp"

#include "rhine/Diagnostic/Diagnostic.hpp"
#include "rhine/IR/Context.hpp"
#include "rhine/IR/Module.hpp"

#include <vector>

#define K Driver->Ctx

namespace rhine {
Parser::Parser(ParseDriver *Dri) : Driver(Dri), CurStatus(true) {}

Parser::~Parser() {}

void Parser::getTok() {
  LastTok = CurTok;
  CurTok = Driver->Lexx->lex(&CurSema, &CurLoc);
  LastTokWasNewlineTerminated = false;
  while (CurTok == NEWLINE) {
    LastTokWasNewlineTerminated = true;
    CurTok = Driver->Lexx->lex(&CurSema, &CurLoc);
  }
}

bool Parser::getTok(int Expected) {
  if (CurTok == Expected) {
    getTok();
    CurLoc.Filename = Driver->InputName;
    CurLoc.StringStreamInput = Driver->StringStreamInput;
    return true;
  }
  return false;
}

void Parser::writeError(std::string ErrStr, bool Optional) {
  if (Optional) return;
  DiagnosticPrinter(CurLoc) << ErrStr;
  CurStatus = false;
}

bool Parser::getSemiTerm(std::string ErrFragment, bool Optional) {
  if (!getTok(';') && !LastTokWasNewlineTerminated && CurTok != ENDBLOCK) {
    auto ErrStr = "expecting ';' or newline to terminate " + ErrFragment;
    writeError(ErrStr, Optional);
    return false;
  }
  return true;
}

void Parser::parseToplevelForms() {
  getTok();
  while (auto Fcn = parseFcnDecl(true)) {
    Driver->Root->appendFunction(Fcn);
  }
  if (CurTok != END)
    writeError("expected end of file");
}

bool Parser::parse() {
  parseToplevelForms();
  return CurStatus;
}
}
