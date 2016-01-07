/* -*- Bison -*- */
%{
#include "rhine/Parse/ParseDriver.hpp"
#include "rhine/Parse/Lexer.hpp"
#include "rhine/IR/Constant.hpp"
#include "rhine/IR/GlobalValue.hpp"
#include "rhine/IR/Context.hpp"

class yyFlexLexer;

#define YY_USER_ACTION yylloc->columns(yyleng);
#define K Driver->Ctx
%}

%option c++ noyywrap warn yylineno stack

SYMBOL  [[:alpha:]][[:alnum:]]*
EXP     [Ee][- +]?[[:digit:]]+
INTEGER [- +]?[[:digit:]]+
SPTAB   [ \t]+

%x str

%%

%{
  yylloc->step();
  char string_buf[500];
  char *string_buf_ptr;
%}


{SPTAB} { yylloc->step(); }

("\r\n"|\n) {
  yylloc->lines(yyleng);
  yylloc->step();
  return T::NEWLINE;
}

{INTEGER} {
  auto C = ConstantInt::get(atoi(yytext), 32, K);
  yylval->Integer = C;
  return T::INTEGER;
}

(\"|\')	string_buf_ptr = string_buf; BEGIN(str);

<str>(\"|\') {
  BEGIN(INITIAL);
  *string_buf_ptr = '\0';
  auto C = GlobalString::get(string_buf, K);
  yylval->String = C;
  return T::STRING;
}

<str>\\n *string_buf_ptr++ = '\n';
<str>\\t *string_buf_ptr++ = '\t';
<str>\\r *string_buf_ptr++ = '\r';
<str>\\b *string_buf_ptr++ = '\b';
<str>\\f *string_buf_ptr++ = '\f';

<str>\\(.|\n) *string_buf_ptr++ = yytext[1];

<str>[^\\\n\"\']+ {
  char *yptr = yytext;
  while (*yptr)
    *string_buf_ptr++ = *yptr++;
}

"//"[^\r\n]* { /* skip comments */ }
"def" { return T::DEF; }
"if" { return T::IF; }
"else" { return T::ELSE; }
"&&" { return T::AND; }
"||" { return T::OR; }
"=" { return T::BIND; }
":=" { return T::MUTATE; }
"->" { return T::ARROW; }
"ret" { return T::RET; }
"do" { return T::DOBLOCK; }
"end" { return T::ENDBLOCK; }
"fn" { return T::LAMBDA; }

"Int" { return T::TINT; }
"Bool" { return T::TBOOL; }
"String" { return T::TSTRING; }
"Void" { return T::TVOID; }
"Function" { return T::TFUNCTION; }
"()" { return T::VOID; }

"true" {
  auto B = ConstantBool::get(true, K);
  yylval->Boolean = B;
  return T::BOOLEAN;
}

"false" {
  auto B = ConstantBool::get(false, K);
  yylval->Boolean = B;
  return T::BOOLEAN;
}

{SYMBOL} {
  yylval->LiteralName = new std::string(yytext);
  return T::LITERALNAME;
}

[\[ \] \( \) + \- * / , ; { } $ ~ = \\ \&] {
  return static_cast<T>(*yytext);
}

. ;

%%

// Required to fill vtable
int yyFlexLexer::yylex()
{
  return 0;
}
