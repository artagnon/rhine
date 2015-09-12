#include "rhine/Parse/Lexer.h"
#include "rhine/Parse/Parser.h"
#include "rhine/Parse/ParseTree.h"
#include "rhine/Parse/ParseDriver.h"
#include "rhine/IR/UnresolvedValue.h"
#include "rhine/IR/GlobalValue.h"
#include "rhine/IR/Instruction.h"
#include "rhine/IR/BasicBlock.h"
#include "rhine/IR/Constant.h"
#include "rhine/IR/Value.h"
#include "rhine/IR/Type.h"

#include <vector>

#define K Driver->Ctx

namespace rhine {
Parser::Parser(ParseDriver *Dri) : Driver(Dri), CurStatus(true) {}

Parser::~Parser() {}

void Parser::getTok() {
  CurTok = Driver->Lexx->lex(&CurSema, &CurLoc);
}

void Parser::writeError(std::string ErrStr, bool Optional) {
  if (Optional) return;
  K->DiagPrinter->errorReport(CurLoc, ErrStr);
  CurStatus = false;
}

Type *Parser::parseOptionalTypeAnnotation() {
  if (CurTok == '~') {
    getTok();
    switch (CurTok) {
    case TINT: {
      auto Ty = IntegerType::get(32, K);
      Ty->setSourceLocation(CurLoc);
      getTok();
      return Ty;
    }
    case TBOOL: {
      auto Ty = BoolType::get(K);
      Ty->setSourceLocation(CurLoc);
      getTok();
      return Ty;
    }
    case TSTRING: {
      auto Ty = StringType::get(K);
      Ty->setSourceLocation(CurLoc);
      getTok();
      return Ty;
    }
    case TFUNCTION: {
      getTok();
      if (CurTok != '(')
        writeError("In function type of form Fn(...), '(' is missing");
      getTok();
      writeError("Cannot parse function types yet");
      while (CurTok != ')')
        getTok();
      getTok();
    }
    case TVOID: {
      auto Ty = VoidType::get(K);
      Ty->setSourceLocation(CurLoc);
      getTok();
      return Ty;
    }
    default:
      writeError("Unrecognized type name");
    }
  }
  return UnType::get(K);
}

std::vector<UnresolvedValue *> Parser::parseArgumentList() {
  std::vector<UnresolvedValue *> ArgumentList;
  while (CurTok != ']') {
    if (CurTok != LITERALNAME)
      writeError("Expected argument name");
    auto Name = *CurSema.LiteralName;
    getTok();
    auto Ty = parseOptionalTypeAnnotation();
    ArgumentList.push_back(UnresolvedValue::get(Name, Ty));
  }
  getTok();
  return ArgumentList;
}

Value *Parser::parseRvalue(bool Optional) {
  switch (CurTok) {
  case INTEGER: {
    auto Int = CurSema.Integer;
    Int->setSourceLocation(CurLoc);
    getTok();
    return Int;
  }
  case BOOLEAN: {
    auto Bool = CurSema.Boolean;
    Bool->setSourceLocation(CurLoc);
    getTok();
    return Bool;
  }
  case STRING: {
    auto Str = CurSema.String;
    Str->setSourceLocation(CurLoc);
    getTok();
    return Str;
  }
  case LITERALNAME: {
    auto RawName = *CurSema.LiteralName;
    auto SymLoc = CurLoc;
    getTok();
    auto Ty = parseOptionalTypeAnnotation();
    auto Sym = UnresolvedValue::get(RawName, Ty);
    Sym->setSourceLocation(SymLoc);
    return Sym;
  }
  default:
    writeError("Expecting an integer, boolean, or string literal");
  }
  return nullptr;
}

Instruction *Parser::parseArithOp(Value *Op0, bool Optional) {
  switch (CurTok) {
  case '+': {
    getTok();
    auto Op1 = parseRvalue();
    return AddInst::get(Op0, Op1);
  }
  case '-': {
    getTok();
    auto Op1 = parseRvalue();
    return SubInst::get(Op0, Op1);
  }
  default:
    writeError("Expected + or -", Optional);
  }
  return nullptr;
}

Value *Parser::parseSingleStm() {
  if (CurTok == IF)
    writeError("Cannot parse if yet");
  else {
    switch (CurTok) {
    case RET: {
      auto RetLoc = CurLoc;
      getTok();
      auto Lit = parseRvalue();
      if (CurTok != ';') {
        writeError("Expecting ; to terminate return statement");
      }
      getTok(); // consume ';', or whatever bad token in its place
      auto Ret = ReturnInst::get(Lit, K);
      Ret->setSourceLocation(RetLoc);
      return Ret;
    }
    case INTEGER:
    case BOOLEAN:
    case STRING: {
      auto Rvalue = parseRvalue();
      return parseArithOp(Rvalue);
    }
    case LITERALNAME: {
      auto Rvalue = parseRvalue();
      if (auto Inst = parseArithOp(Rvalue, true))
        return Inst;
      writeError("Cannot parse function calls yet");
    }
    default:
      writeError("Expecting a single statement");
    }
  }
  return nullptr;
}

BasicBlock *Parser::parseCompoundBody() {
  std::vector<Value *> StmList;

  while (CurTok != '}' && CurTok != END)
    StmList.push_back(parseSingleStm());
  if (CurTok == END) {
    writeError("Dangling compound body");
  }
  return BasicBlock::get("entry", StmList, K);
}

Function *Parser::parseFnDecl() {
  if (CurTok != DEF) {
    writeError("Expected 'def', to begin function definition");
  }
  getTok();
  if (CurTok != LITERALNAME) {
    writeError("Expected function name");
  }
  auto FcnName = *CurSema.LiteralName;
  auto FcnLoc = CurLoc;
  getTok();
  if (CurTok != '[') {
    writeError("Expected '[' to start function argument list");
  }
  getTok(); // consume '['
  auto ArgList = parseArgumentList();
  auto Ty = parseOptionalTypeAnnotation();
  auto FTy = FunctionType::get(Ty, K);
  auto Fcn = Function::get(FcnName, FTy);
  Fcn->setSourceLocation(FcnLoc);

  if (CurTok == '{') {
    getTok(); // consume '{'
    Fcn->push_back(parseCompoundBody());
  } else {
    Fcn->push_back(BasicBlock::get("entry", { parseSingleStm() }, K));
  }
  return Fcn;
}

void Parser::parseToplevelForms() {
  getTok();
  Driver->Root.appendFunction(parseFnDecl());
}

bool Parser::parse() {
  parseToplevelForms();
  return CurStatus;
}
}
