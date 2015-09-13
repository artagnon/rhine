/* -*- Bison -*- */
%{
#include "rhine/Parse/ParseDriver.h"
#include "rhine/Parse/Lexer.h"
#include "rhine/IR/Constant.h"
#include "rhine/IR/GlobalValue.h"

class yyFlexLexer;

#define YY_USER_ACTION yylloc->columns(yyleng);
#define K Driver->Ctx
%}

%option c++ noyywrap warn yylineno stack

SYMBOL  [[:alpha:]][[:alnum:]]*
EXP     [Ee][- +]?[[:digit:]]+
INTEGER [- +]?[[:digit:]]+
RET     [\r\n]+
SPTAB   [ \t]+

%x str

%%

%{
  yylloc->step();
  char string_buf[500];
  char *string_buf_ptr;
%}


{SPTAB} { yylloc->step(); }

{RET} { yylloc->lines(yyleng); yylloc->step(); }

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
"->" { return T::ARROW; }
"ret" { return T::RET; }
"do" { return T::DO; }
"end" { return T::ENDBLOCK; }

"Int" { return T::TINT; }
"Bool" { return T::TBOOL; }
"String" { return T::TSTRING; }
"Void" { return T::TVOID; }
"Fn" { return T::TFUNCTION; }

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
  yylval->LiteralName = new (K->RhAllocator) std::string(yytext);
  return T::LITERALNAME;
}

[\[ \] \( \) + \- * ; { } $ ~ = \\ \&] {
  return static_cast<T>(*yytext);
}

. ;

%%

// Required to fill vtable
int yyFlexLexer::yylex()
{
  return 0;
}
