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
#define writeError(Str) K->DiagPrinter->errorReport(CurLoc, Str)

namespace rhine {
Parser::Parser(ParseDriver *Dri) : Driver(Dri), CurStatus(true) {}

Parser::~Parser() {}

void Parser::getTok() {
  CurTok = Driver->Lexx->lex(&CurSema, &CurLoc);
}

Type *Parser::parseOptionalTypeAnnotation() {
  getTok();
  if (CurTok == '~') {
    writeError("Cannot parse type yet");
    CurStatus = false;
  }
  return UnType::get(K);
}

std::vector<UnresolvedValue *> Parser::parseArgumentList() {
  std::vector<UnresolvedValue *> ArgumentList;

  getTok();
  while (CurTok != ']') {
    if (CurTok != LITERALNAME) {
      writeError("Expected argument name");
      CurStatus = false;
    }
    auto Name = *CurSema.LiteralName;
    auto Ty = parseOptionalTypeAnnotation();
    ArgumentList.push_back(UnresolvedValue::get(Name, Ty));
  }
  return ArgumentList;
}

Value *Parser::parseLiteral() {
  getTok();
  switch (CurTok) {
  case INTEGER: {
    auto Int = CurSema.Integer;
    Int->setSourceLocation(CurLoc);
    return Int;
  }
  case BOOLEAN: {
    auto Bool = CurSema.Boolean;
    Bool->setSourceLocation(CurLoc);
    return Bool;
  }
  case STRING: {
    auto Str = CurSema.String;
    Str->setSourceLocation(CurLoc);
    return Str;
  }
  default: {
    writeError("Expecting an integer, boolean, or string literal");
    CurStatus = false;
  }
  }
  return nullptr;
}

Value *Parser::parseSingleStm() {
  // we do not start with a getTok() because
  // 1. when called from main, parseOptionalArgument() getToks for us
  // 2. when called from compoundBody, a getTok() is called for us
  if (CurTok == IF) {
    writeError("Cannot parse if yet");
    CurStatus = false;
  } else {
    switch (CurTok) {
    case RET: {
      auto RetLoc = CurLoc;
      auto Lit = parseLiteral();
      auto Ret = ReturnInst::get(Lit, K);
      Ret->setSourceLocation(RetLoc);
      return Ret;
    }
    case LITERALNAME: {
      writeError("Cannot parse function calls yet");
      CurStatus = false;
    }
    default: {
      writeError("Expecting a single statement");
      CurStatus = false;
    }
    }
  }
  return nullptr;
}

BasicBlock *Parser::parseCompoundBody() {
  std::vector<Value *> StmList;

  getTok(); // look at the token after '{'
  while (CurTok != '}' && CurTok != END) {
    StmList.push_back(parseSingleStm());
    getTok();
  }
  if (CurTok == END) {
    writeError("Dangling compound body");
    CurStatus = false;
  }
  return BasicBlock::get("entry", StmList, K);
}

Function *Parser::parseFnDecl() {
  getTok();
  if (CurTok != DEF) {
    writeError("Expected 'def', to begin function definition");
    CurStatus = false;
  }
  getTok();
  if (CurTok != LITERALNAME) {
    writeError("Expected function name");
    CurStatus = false;
  }
  auto FcnName = *CurSema.LiteralName;
  auto FcnLoc = CurLoc;
  getTok();
  if (CurTok != '[') {
    writeError("Expected '[' to start function argument list");
    CurStatus = false;
  }
  auto ArgList = parseArgumentList();
  auto Ty = parseOptionalTypeAnnotation();
  auto FTy = FunctionType::get(Ty, K);
  auto Fcn = Function::get(FcnName, FTy);
  Fcn->setSourceLocation(FcnLoc);
  if (CurTok == '{')
    Fcn->push_back(parseCompoundBody());
  else {
    // looking at a token that begins a stm
    Fcn->push_back(BasicBlock::get("entry", { parseSingleStm() }, K));
  }
  return Fcn;
}

void Parser::parseToplevelForms() {
  Driver->Root.appendFunction(parseFnDecl());
}

bool Parser::parse() {
  parseToplevelForms();
  return CurStatus;
}
}
