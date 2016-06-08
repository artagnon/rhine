#pragma once

// Flex expects the signature of yylex to be defined in the macro YY_DECL, and
// the C++ parser expects it to be declared. We can factor both as follows.

#include "rhine/Parse/ParseDriver.hpp"
#include "rhine/Parse/Parser.hpp"

using P = rhine::Parser;
using T = P::Token;

#define yyterminate() return T::END

#define YY_DECL T rhine::Lexer::lex(P::Semantic *yylval, P::Location *yylloc)

#ifndef __FLEX_LEXER_H
#include <FlexLexer.h>
#endif

/** Scanner is a derived class to add some extra function to the scanner
 * class. Flex itself creates a class named yyFlexLexer. However we change the
 * context of the generated yylex() function to be contained within the Lexer
 * class. This is required because the yylex() defined in yyFlexLexer has no
 * parameters. */
namespace rhine {
class Lexer : public yyFlexLexer {
public:
  Lexer(std::istream &arg_yyin, std::ostream &arg_yyout, ParseDriver *Dri)
      : yyFlexLexer(arg_yyin, arg_yyout), Driver(Dri) {}
  void LexerError(const char msg[]) { yyout << msg << std::endl; }
  virtual ~Lexer() {}
  virtual T lex(P::Semantic *yylval, P::Location *yylloc);
  ParseDriver *Driver;
};
}
