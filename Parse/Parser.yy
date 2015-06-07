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
  std::vector<class Value *> *ValueList;
  std::vector<class Type *> *TypeList;
}

%start start

%token                  DEF
%token                  IF
%token                  THEN
%token                  AND
%token                  OR
%token                  ARROW
%token                  TINT
%token                  TBOOL
%token                  TSTRING
%token                  TFUNCTION
%token                  END       0
%token  <RawSymbol>     SYMBOL
%token  <Integer>       INTEGER
%token  <Boolean>       BOOLEAN
%token  <String>        STRING
%type   <VarList>       argument_list
%type   <ValueList>     compound_stm stm_list single_stm rvalue_list
%type   <Fcn>           fn_decl def
%type   <Value>         expression assign_expr value_expr rvalue
%type   <Type>          type_annotation type_lit
%type   <TypeList>      type_list
%type   <Symbol>        typed_symbol lvalue

%{
#include "rhine/Parse/Lexer.h"
#include "rhine/Parse/ParseDriver.h"
#include "rhine/Diagnostic.h"

#undef yylex
#define yylex Driver->Lexx->lex
#define K Driver->Ctx
%}

%%

start:
        |       tlexpr start END
                ;


tlexpr:
                def[D]
                {
                  Driver->Root.M.appendFunction($D);
                }
                ;

fn_decl:
                DEF SYMBOL[N] '[' argument_list[A] ']' type_annotation[T]
                {
                  std::vector<Type *> ATys;
                  for (auto Sym : *$A)
                    ATys.push_back(Sym->getType());
                  auto FTy = FunctionType::get($T, ATys, K);
                  FTy->setSourceLocation(@4);
                  auto Fn = Function::get(FTy, K);
                  Fn->setSourceLocation(@1);
                  Fn->setName(*$N);
                  Fn->setArguments(*$A);
                  $$ = Fn;
                }
        |       DEF SYMBOL[N] '[' ']' type_annotation[T]
                {
                  auto FTy = FunctionType::get($T, K);
                  FTy->setSourceLocation(@3);
                  auto Fn = Function::get(FTy, K);
                  Fn->setSourceLocation(@1);
                  Fn->setName(*$N);
                  $$ = Fn;
                }
                ;
def:
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
        |       '~' type_lit[T]
                {
                  $$ = $T;
                }
                ;
type_lit:
                TINT
                {
                  auto ITy = IntegerType::get(32, K);
                  ITy->setSourceLocation(@1);
                  $$ = ITy;
                }
        |       TBOOL
                {
                  auto BTy = BoolType::get(K);
                  BTy->setSourceLocation(@1);
                  $$ = BTy;
                }
                ;
        |       TSTRING
                {
                  auto STy = StringType::get(K);
                  STy->setSourceLocation(@1);
                  $$ = STy;
                }
        |       TFUNCTION '(' type_list[A] ARROW type_lit[R] ')'
                {
                  auto FTy = FunctionType::get($R, *$A, K);
                  auto PTy = PointerType::get(FTy, K);
                  FTy->setSourceLocation(@1);
                  PTy->setSourceLocation(@1);
                  $$ = PTy;
                }
                ;
type_list:
                type_lit[T]
                {
                  auto TypeList = new (K->RhAllocator) std::vector<Type *>;
                  TypeList->push_back($T);
                  $$ = TypeList;
                }
        |       type_list[L] ARROW type_lit[T]
                {
                  $L->push_back($T);
                  $$ = $L;
                }
                ;
expression:
                value_expr[V]
                {
                  $$ = $V;
                }
        |       assign_expr[A]
                {
                  $$ = $A;
                }
        |       IF value_expr[V] THEN compound_stm[T] compound_stm[F]
                {
                  $$ = nullptr;
                }
        |       IF assign_expr[A] THEN compound_stm[T] compound_stm[F]
                {
                  $$ = nullptr;
                }
                ;
value_expr:
                rvalue[V]
                {
                  $$ = $V;
                }
        |       rvalue[L] '+' rvalue[R]
                {
                  auto Op = AddInst::get(K);
                  Op->addOperand($L);
                  Op->addOperand($R);
                  $$ = Op;
                }
        |       typed_symbol[S] rvalue_list[L]
                {
                  auto CInst = CallInst::get($S->getName(), K);
                  CInst->setSourceLocation(@1);
                  for (auto Op: *$L)
                    CInst->addOperand(Op);
                  $$ = CInst;
                }
        |       typed_symbol[S] '$' value_expr[E]
                {
                  auto Op = CallInst::get($S->getName(), K);
                  Op->setSourceLocation(@1);
                  Op->addOperand($E);
                  $$ = Op;
                }
        |       '\\' argument_list[A] ARROW compound_stm[B]
                {
                  std::vector<Type *> ATys;
                  for (auto Sym : *$A)
                    ATys.push_back(Sym->getType());
                  auto FTy = FunctionType::get(UnType::get(K), ATys, K);
                  FTy->setSourceLocation(@4);
                  auto Fn = Function::get(FTy, K);
                  Fn->setSourceLocation(@1);
                  Fn->setArguments(*$A);
                  Fn->setBody(*$B);
                  $$ = Fn;
                }
                ;
assign_expr:
                lvalue[L] '=' value_expr[E]
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
                ;
rvalue_list:
                rvalue[R]
                {
                  auto RValueList = new (K->RhAllocator) std::vector<Value *>;
                  RValueList->push_back($R);
                  $$ = RValueList;
                }
        |       rvalue_list[L] rvalue[R]
                {
                  $L->push_back($R);
                  $$ = $L;
                }
                ;

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
