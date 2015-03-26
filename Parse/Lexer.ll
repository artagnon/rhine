/* -*- Bison -*- */
%{
#include "rhine/ParseDriver.h"
#include "rhine/Lexer.h"

#define YY_USER_ACTION yylloc->columns(yyleng);
%}

%option c++ noyywrap nodefault warn yylineno stack

%option warn nodefault

SYMBOLC [a-z A-Z ? - * / < > = . % ^]
SYMBOL  [[:alpha:]][[:alnum:]]+
EXP     [Ee][- +]?[[:digit:]]+
INTEGER [- +]?[[:digit:]]+
STRING  \".*\"
RET     [\r\n]+
SPTAB   [ \t]+

%%

%{
  yylloc->step();
%}

{SPTAB} { yylloc->step(); }

{RET} { yylloc->lines(yyleng); yylloc->step(); }

{INTEGER} {
  auto C = ConstantInt::get(atoi(yytext));
  yylval->Integer = C;
  return T::INTEGER;
}

{STRING} {
  auto C = ConstantString::get(yytext);
  yylval->String = C;
  return T::STRING;
}

"defun" { return T::DEFUN; }
"if" { return T::IF; }
"then" { return T::THEN; }

{SYMBOL} {
  yylval->RawSymbol = new std::string(yytext, yyleng);
  return T::SYMBOL;
}

[\[ \] \( \) + * ; { } $] {
  return static_cast<P::token_type>(*yytext);
}

. ;

%%

// Required to fill vtable
int yyFlexLexer::yylex()
{
  return 0;
}
