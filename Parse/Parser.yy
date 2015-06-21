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
  class BasicBlockSpear *BBSpear;
  class Function *Fcn;
  class Value *Value;
  class Type *Type;
  std::vector<class BasicBlock *> *BBList;
  std::vector<class Symbol *> *VarList;
  std::vector<class Value *> *ValueList;
  std::vector<class Type *> *TypeList;
}

%start start

%token                  DEF IF ELSE AND OR ARROW
%token                  TINT TBOOL TSTRING TFUNCTION
%token                  END       0
%token  <RawSymbol>     SYMBOL
%token  <Integer>       INTEGER
%token  <Boolean>       BOOLEAN
%token  <String>        STRING
%type   <VarList>       argument_list
%type   <BBSpear>       stm_list
%type   <BBList>        compound_stm
%type   <ValueList>     rvalue_list expression_list
%type   <Fcn>           fn_decl def
%type   <Value>         expression assign_expr value_expr rvalue
%type   <Type>          type_annotation type_lit
%type   <TypeList>      type_list
%type   <Symbol>        typed_symbol lvalue

%{
#include "rhine/Parse/Lexer.h"
#include "rhine/Parse/ParseDriver.h"
#include "rhine/Parse/ParseTree.h"
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
                  auto FTy = FunctionType::get($T, ATys, false, K);
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
                  $F->setBody((*$L)[0]);
                  $$ = $F;
                }
                ;
compound_stm:
                '{' stm_list[L] '}'
                {
                  $$ = $L->getBBs();
                }
        |       expression[E] ';'
                {
                  std::vector<Value *> ExpressionList;
                  ExpressionList.push_back($E);
                  auto BBList = new (K->RhAllocator) std::vector<BasicBlock *>;
                  BBList->push_back(BasicBlock::get(ExpressionList, K));
                  $$ = BBList;
                }
stm_list:
                expression_list[L]
                {
                  std::vector<BasicBlock *> BBList;
                  BBList.push_back(BasicBlock::get(*$L, K));
                  auto BBSpear = BasicBlockSpear::get(nullptr, BBList, K);
                  $$ = BBSpear;
                }
        |       IF '(' value_expr[V] ')' compound_stm[T] ELSE compound_stm[F]
                {
                  auto TrueBB = (*$T)[0];
                  auto FalseBB = (*$F)[0];
                  std::vector<BasicBlock *> BBList;
                  BBList.push_back(TrueBB);
                  BBList.push_back(FalseBB);
                  auto IfStmt = IfInst::get($V, TrueBB, FalseBB, K);
                  auto BBSpear = BasicBlockSpear::get(IfStmt, BBList, K);
                  $$ = BBSpear;
                }
        |       IF '(' assign_expr[A] ')' compound_stm[T] ELSE compound_stm[F]
                {
                  $$ = nullptr;
                }
                ;
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
                  auto FTy = FunctionType::get($R, *$A, false, K);
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
expression_list:
                expression[E] ';'
                {
                  auto EList = new (K->RhAllocator) std::vector<Value *>;
                  EList->push_back($E);
                  $$ = EList;
                }
        |       expression_list[L] expression[E] ';'
                {
                  $L->push_back($E);
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
                ;
value_expr:
                rvalue[V]
                {
                  $$ = $V;
                }
        |       rvalue[L] '+' rvalue[R]
                {
                  auto Op = AddInst::get(K);
                  Op->setSourceLocation(@1);
                  Op->addOperand($L);
                  Op->addOperand($R);
                  $$ = Op;
                }
        |       typed_symbol[S] '(' ')'
                {
                  auto CInst = CallInst::get($S->getName(), K);
                  CInst->setSourceLocation(@1);
                  $$ = CInst;
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
                  auto FTy = FunctionType::get(UnType::get(K), ATys, false, K);
                  FTy->setSourceLocation(@4);
                  auto Fn = Function::get(FTy, K);
                  Fn->setSourceLocation(@1);
                  Fn->setArguments(*$A);
                  Fn->setBody((*$B)[0]);
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
