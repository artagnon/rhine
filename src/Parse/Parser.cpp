#include "rhine/Parse/ParseDriver.h"
#include "rhine/Parse/Parser.h"
#include "rhine/Parse/Lexer.h"

#include "rhine/Diagnostic/Diagnostic.h"
#include "rhine/IR/Context.h"
#include "rhine/IR/Module.h"

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
  auto Ret = CurTok == Expected;
  if (Ret) {
    getTok();
    CurLoc.Begin.Filename = CurLoc.End.Filename = Driver->StreamName;
  }
  return Ret;
}

void Parser::writeError(std::string ErrStr, bool Optional) {
  if (Optional) return;
  K->DiagPrinter->errorReport(CurLoc, ErrStr);
  CurStatus = false;
}

void Parser::getSemiTerm(std::string ErrFragment) {
  if (!getTok(';') && !LastTokWasNewlineTerminated && CurTok != ENDBLOCK)
    writeError("expecting ';' or newline to terminate " + ErrFragment);
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
