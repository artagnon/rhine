#include "rhine/Parse/Lexer.h"
#include "rhine/Parse/Parser.h"
#include "rhine/Parse/ParseDriver.h"

namespace rhine {
Parser::Parser(ParseDriver *Dri) : Driver(Dri) {}

int Parser::parse() {
  Semantic Sema;
  Location Loc;
  return Driver->Lexx->lex(&Sema, &Loc);
}
}
