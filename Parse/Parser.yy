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
  class ConstantInt *Integer;
  class ConstantBool *Boolean;
  class ConstantFloat *Float;
  class GlobalString *String;
  class Value *Value;
  class Instruction *Inst;
  class Function *Fcn;
  std::vector<class Variable *> *VarList;
  std::vector<class Value *> *StmList;
}

%start start

%token                  DEFUN
%token                  IF
%token                  THEN
%token                  AND
%token                  OR
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
                DEFUN SYMBOL[N] '[' argument_list[A] ']'
                {
                  auto ITy = IntegerType::get();
                  auto FTy = FunctionType::get(ITy);
                  auto Fn = Function::get(FTy);
                  Fn->setName(*$N);
                  Fn->setArgumentList(*$A);
                  $$ = Fn;
                }
        |       DEFUN SYMBOL[N] '[' ']'
                {
                  auto ITy = IntegerType::get();
                  auto FTy = FunctionType::get(ITy);
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
                SYMBOL[S]
                {
                  auto SymbolList = new std::vector<Variable *>;
                  auto Sym = Variable::get(*$S);
                  SymbolList->push_back(Sym);
                  $$ = SymbolList;
                }
        |       argument_list[L] SYMBOL[S]
                {
                  auto Sym = Variable::get(*$S);
                  $L->push_back(Sym);
                  $$ = $L;
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
        |       SYMBOL[S] STRING[P]
                {
                  auto Op = CallInst::get(*$S);
                  Op->addOperand($P);
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
        |       SYMBOL[S]
                {
                  $$ = Variable::get(*$S);
                }
                ;
%%

void rhine::Parser::error(const rhine::location &l,
                          const std::string &m)
{
  Driver->error(l, m);
}
