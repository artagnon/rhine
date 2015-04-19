// -*- mode: bison -*-
%{
#include <iostream>
%}

%debug
%name-prefix "rhine"
%skeleton "lalr1.cc"
%locations

%initial-action
{
    @$.begin.filename = @$.end.filename = &Driver->StreamName;
};

%token-table
%define parser_class_name { Parser }
%defines

%parse-param { class ParseDriver *Driver }
%error-verbose

%union {
  std::string *RawSymbol;
  class Symbol *Symbol;
  class ConstantInt *Integer;
  class ConstantBool *Boolean;
  class ConstantFloat *Float;
  class GlobalString *String;
  class Function *Fcn;
  class Value *Value;
  class Type *Type;
  std::vector<class Symbol *> *VarList;
  std::vector<class Value *> *StmList;
}

%start start

%token                  DEFUN
%token                  IF
%token                  THEN
%token                  AND
%token                  OR
%token                  TINT
%token                  TBOOL
%token                  TSTRING
%token                  END       0
%token  <RawSymbol>     SYMBOL
%token  <Integer>       INTEGER
%token  <Boolean>       BOOLEAN
%token  <String>        STRING
%type   <VarList>       argument_list
%type   <StmList>       compound_stm stm_list single_stm
%type   <Fcn>           fn_decl defun
%type   <Value>         expression
%type   <Value>         rvalue
%type   <Type>          type_annotation
%type   <Symbol>        typed_symbol

%destructor { delete $$; } SYMBOL INTEGER STRING
%destructor { delete $$; } argument_list stm_list
%destructor { delete $$; } fn_decl defun
%destructor { delete $$; } expression rvalue

%{
#include "rhine/ParseDriver.h"
#include "rhine/Lexer.h"

#undef yylex
#define yylex Driver->Lexx->lex
%}

%%

start:
        |       tlexpr start END
                ;


tlexpr:
                defun[D]
                {
                  Driver->Root.M.appendFunction($D);
                }
                ;

fn_decl:
                DEFUN SYMBOL[N] '[' argument_list[A] ']' type_annotation[T]
                {
                  auto FTy = FunctionType::get($T, Driver->Ctx);
                  auto Fn = Function::get(FTy);
                  Fn->setName(*$N);
                  Fn->setArgumentList(*$A);
                  $$ = Fn;
                }
        |       DEFUN SYMBOL[N] '[' ']' type_annotation[T]
                {
                  auto FTy = FunctionType::get($T, Driver->Ctx);
                  auto Fn = Function::get(FTy);
                  Fn->setName(*$N);
                  $$ = Fn;
                }
                ;
defun:
                fn_decl[F] compound_stm[L]
                {
                  $F->setBody(*$L);
                  $$ = $F;
                }
                ;
compound_stm:
                '{' stm_list[L] '}'
                {
                  $$ = $L;
                }
        |       single_stm[L]
                {
                  $$ = $L;
                }
stm_list:
                single_stm[L]
                {
                  $$ = $L;
                }
        |       stm_list[L] expression[E] ';'
                {
                  $L->push_back($E);
                  $$ = $L;
                }
                ;
single_stm:
                expression[E] ';'
                {
                  auto StatementList = new std::vector<Value *>;
                  StatementList->push_back($E);
                  $$ = StatementList;
                }
argument_list:
                typed_symbol[S]
                {
                  auto SymbolList = new std::vector<Symbol *>;
                  SymbolList->push_back($S);
                  $$ = SymbolList;
                }
        |       argument_list[L] typed_symbol[S]
                {
                  $L->push_back($S);
                  $$ = $L;
                }
                ;
typed_symbol:
                SYMBOL[S] type_annotation[T]
                {
                  $$ = Symbol::get(*$S, $T, Driver->Ctx);
                }
        ;
type_annotation:
                {
                  $$ = UnType::get();
                }
        |       '~' TINT
                {
                  $$ = IntegerType::get();
                }
        |       '~' TBOOL
                {
                  $$ = BoolType::get();
                }
                ;
        |       '~' TSTRING
                {
                  $$ = StringType::get();
                }
                ;
expression:
                rvalue[V]
                {
                  $$ = $V;
                }
        |       rvalue[L] '+' rvalue[R]
                {
                  auto Op = AddInst::get(IntegerType::get());
                  Op->addOperand($L);
                  Op->addOperand($R);
                  $$ = Op;
                }
        |       typed_symbol[S] rvalue[R]
                {
                  auto Op = CallInst::get($S->getName());
                  Op->addOperand($R);
                  $$ = Op;
                }
        |       IF expression[C] THEN compound_stm[T] compound_stm[F]
                {
                  $$ = nullptr;
                }
                ;
rvalue:
                INTEGER[I]
                {
                  $$ = $I;
                }
        |       STRING[P]
                {
                  $$ = $P;
                }
        |       typed_symbol[S]
                {
                  $$ = $S;
                }
                ;
%%

void rhine::Parser::error(const rhine::location &l,
                          const std::string &m)
{
  Driver->error(l, m);
}
