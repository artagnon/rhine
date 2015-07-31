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
  std::string *LiteralName;
  class LoadInst *MallocedVar;
  class Argument *Argument;
  class ConstantInt *Integer;
  class ConstantBool *Boolean;
  class ConstantFloat *Float;
  class GlobalString *String;
  class BasicBlock *BB;
  class Function *Fcn;
  class Value *Value;
  class Type *Type;
  class ArgumentList *ArgList;
  class TypeList *TyList;
  std::vector<class Value *> *ValueList;
}

%start start

%token                  DEF IF ELSE AND OR ARROW
%token                  TINT TBOOL TSTRING TFUNCTION TVOID
%token                  END       0
%token  <LiteralName>   LITERALNAME
%token  <Integer>       INTEGER
%token  <Boolean>       BOOLEAN
%token  <String>        STRING
%type   <ArgList>       argument_list
%type   <BB>            compound_stm
%type   <ValueList>     rvalue_list stm_list
%type   <Fcn>           fn_decl def
%type   <Value>         expression expression_or_branch
%type   <Value>         assign_expr value_expr rvalue
%type   <Value>         lambda_assign lambda_expr
%type   <Type>          type_annotation type_lit
%type   <TyList>        type_list
%type   <Argument>      typed_argument
%type   <MallocedVar>   typed_symbol lvalue

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
                DEF LITERALNAME[N] '[' argument_list[A] ']' type_annotation[T]
                {
                  std::vector<Type *> ATys;
                  Argument *VariadicRest = nullptr;
                  if ($A->isVariadic()) {
                    VariadicRest = $A->back();
                    $A->pop_back();
                  }
                  for (auto Sym : *$A)
                    ATys.push_back(Sym->getType());
                  auto FTy = FunctionType::get($T, ATys, VariadicRest, K);
                  FTy->setSourceLocation(@4);
                  auto Fn = Function::get(FTy, K);
                  Fn->setSourceLocation(@1);
                  Fn->setName(*$N);
                  Fn->setArguments(*$A);
                  Fn->setVariadicRest(VariadicRest);
                  $$ = Fn;
                }
        |       DEF LITERALNAME[N] '[' ']' type_annotation[T]
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
                  $F->setBody($L);
                  $$ = $F;
                }
                ;
compound_stm:
                '{' stm_list[L] '}'
                {
                  $$ = BasicBlock::get(*$L, K);
                }
        |       expression_or_branch[E]
                {
                  std::vector<Value *> StmList;
                  StmList.push_back($E);
                  $$ = BasicBlock::get(StmList, K);
                }
argument_list:
                typed_argument[S]
                {
                  auto AList = new (K->RhAllocator) ArgumentList;
                  AList->push_back($S);
                  $$ = AList;
                }
        |       argument_list[L] typed_argument[S]
                {
                  $L->push_back($S);
                  $$ = $L;
                }
        |       argument_list[L] '&' typed_argument[S]
                {
                  $L->push_back($S);
                  $L->setIsVariadic(true);
                  $$ = $L;
                }
                ;
typed_argument:
                LITERALNAME[S] type_annotation[T]
                {
                  auto Sym = Argument::get(*$S, $T, K);
                  Sym->setSourceLocation(@1);
                  $$ = Sym;
                }
                ;
typed_symbol:
                LITERALNAME[S] type_annotation[T]
                {
                  auto Sym = LoadInst::get(*$S, $T, K);
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
        |       TVOID
                {
                  auto STy = VoidType::get(K);
                  STy->setSourceLocation(@1);
                  $$ = STy;
                }
        |       TFUNCTION '(' type_list[A] ARROW type_lit[R] ')'
                {
                  auto FTy = FunctionType::get($R, *$A, $A->isVariadic(), K);
                  auto PTy = PointerType::get(FTy, K);
                  FTy->setSourceLocation(@1);
                  PTy->setSourceLocation(@1);
                  $$ = PTy;
                }
                ;
type_list:
                type_lit[T]
                {
                  auto TyL = new (K->RhAllocator) TypeList;
                  TyL->push_back($T);
                  $$ = TyL;
                }
        |       type_list[L] ARROW type_lit[T]
                {
                  $L->push_back($T);
                  $$ = $L;
                }
        |       type_list[L] ARROW '&'
                {
                  $L->setIsVariadic(true);
                  $$ = $L;
                }
                ;
stm_list:
                {
                  $$ = nullptr;
                }
        |       stm_list[L] expression_or_branch[E]
                {
                  if ($1 == nullptr) {
                    auto EList = new (K->RhAllocator) std::vector<Value *>;
                    EList->push_back($E);
                    $$ = EList;
                  } else {
                    $1->push_back($E);
                    $$ = $1;
                  }
                }
                ;
expression_or_branch:
                expression[E] ';'
                {
                  $$ = $E;
                }
        |       lambda_assign[A]
                {
                  $$ = $A;
                }
        |       IF '(' value_expr[V] ')' compound_stm[T] ELSE compound_stm[F]
                {
                  $$ = IfInst::get($V, $T, $F, K);
                }
        |       IF '(' assign_expr[A] ')' compound_stm[T] ELSE compound_stm[F]
                {
                  $$ = IfInst::get($A, $T, $F, K);
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
        |       typed_symbol[S] TVOID
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
                ;
assign_expr:
                lvalue[L] '=' value_expr[E]
                {
                  auto Op = MallocInst::get($L->getName(), $E, K);
                  Op->setSourceLocation(@1);
                  $$ = Op;
                }
                ;
lambda_assign:
                lvalue[L] '=' lambda_expr[E]
                {
                  auto Op = MallocInst::get($L->getName(), $E, K);
                  Op->setSourceLocation(@1);
                  $$ = Op;
                }
                ;
lambda_expr:
                '\\' argument_list[A] ARROW compound_stm[B]
                {
                  std::vector<Type *> ATys;
                  for (auto Sym : *$A)
                    ATys.push_back(Sym->getType());
                  auto FTy = FunctionType::get(UnType::get(K), ATys, false, K);
                  FTy->setSourceLocation(@4);
                  auto Fn = Function::get(FTy, K);
                  Fn->setSourceLocation(@1);
                  Fn->setArguments(*$A);
                  Fn->setBody($B);
                  $$ = Fn;
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
        |       BOOLEAN[B]
                {
                  auto Bool = $B;
                  Bool->setSourceLocation(@1);
                  $$ = Bool;
                }
        |       STRING[P]
                {
                  auto Str = $P;
                  Str->setSourceLocation(@1);
                  $$ = Str;
                }
        |       lvalue[L]
                {
                  $$ = $L;
                }
                ;
%%

void rhine::Parser::error(const rhine::location &l,
                          const std::string &m)
{
  K->DiagPrinter->errorReport(l, m);
}
