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
%type   <Value>         assign_expr
%type   <Value>         rvalue
%type   <Type>          type_annotation
%type   <Symbol>        typed_symbol
%type   <Symbol>        lvalue

%{
#include "rhine/ParseDriver.h"
#include "rhine/Diagnostic.h"
#include "rhine/Lexer.h"

#undef yylex
#define yylex Driver->Lexx->lex
#define K Driver->Ctx
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
                  std::vector<Type *> ATys;
                  for (auto Sym : *$A)
                    ATys.push_back(Sym->getType());
                  auto FTy = FunctionType::get($T, ATys, K);
                  FTy->setSourceLocation(@4);
                  auto Fn = Function::get(FTy, K);
                  Fn->setSourceLocation(@1);
                  Fn->setName(*$N);
                  Fn->setArgumentList(*$A);
                  $$ = Fn;
                }
        |       DEFUN SYMBOL[N] '[' ']' type_annotation[T]
                {
                  auto FTy = FunctionType::get($T, K);
                  FTy->setSourceLocation(@3);
                  auto Fn = Function::get(FTy, K);
                  Fn->setSourceLocation(@1);
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
                  auto StatementList = new (K->RhAllocator) std::vector<Value *>;
                  StatementList->push_back($E);
                  $$ = StatementList;
                }
argument_list:
                typed_symbol[S]
                {
                  auto SymbolList = new (K->RhAllocator) std::vector<Symbol *>;
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
                  auto Sym = Symbol::get(*$S, $T, K);
                  Sym->setSourceLocation(@1);
                  $$ = Sym;
                }
        ;
type_annotation:
                {
                  $$ = UnType::get(K);
                }
        |       '~' TINT
                {
                  auto ITy = IntegerType::get(K);
                  ITy->setSourceLocation(@2);
                  $$ = ITy;
                }
        |       '~' TBOOL
                {
                  auto BTy = BoolType::get(K);
                  BTy->setSourceLocation(@2);
                  $$ = BTy;
                }
                ;
        |       '~' TSTRING
                {
                  auto STy = StringType::get(K);
                  STy->setSourceLocation(@2);
                  $$ = STy;
                }
                ;
expression:
                rvalue[V]
                {
                  $$ = $V;
                }
        |       rvalue[L] '+' rvalue[R]
                {
                  auto Op = AddInst::get(IntegerType::get(K), K);
                  Op->addOperand($L);
                  Op->addOperand($R);
                  $$ = Op;
                }
        |       assign_expr[A]
                {
                  $$ = $A;
                }
        |       typed_symbol[S] rvalue[R]
                {
                  // FIXME: IntegerType should be UnType and let type inference
                  // do its job
                  auto Op = CallInst::get($S->getName(), IntegerType::get(K), K);
                  Op->setSourceLocation(@1);
                  Op->addOperand($R);
                  $$ = Op;
                }
        |       IF expression[C] THEN compound_stm[T] compound_stm[F]
                {
                  $$ = nullptr;
                }
                ;
assign_expr:
                lvalue[L] '=' expression[E]
                {
                  auto Op = BindInst::get($L->getName(), $E, K);
                  Op->setSourceLocation(@1);
                  $$ = Op;
                }
                ;
lvalue:
                typed_symbol[S]
                {
                  $$ = $S;
                }
rvalue:
                INTEGER[I]
                {
                  auto Int = $I;
                  Int->setSourceLocation(@1);
                  $$ = Int;
                }
        |       STRING[P]
                {
                  auto Str = $P;
                  Str->setSourceLocation(@1);
                  $$ = Str;
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
  K->DiagPrinter->errorReport(l, m);
}
