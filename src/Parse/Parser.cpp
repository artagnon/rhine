#include "rhine/Parse/Lexer.h"
#include "rhine/Parse/Parser.h"
#include "rhine/Parse/ParseTree.h"
#include "rhine/Parse/ParseDriver.h"
#include "rhine/IR/UnresolvedValue.h"
#include "rhine/IR/BasicBlock.h"
#include "rhine/IR/Constant.h"
#include "rhine/IR/Value.h"
#include "rhine/IR/Type.h"

#include <vector>

#define K Driver->Ctx
#define writeError(Str) K->DiagPrinter->errorReport(SavedLoc, Str)

namespace rhine {
Parser::Parser(ParseDriver *Dri) : Driver(Dri), CurStatus(true) {}

Parser::~Parser() {}

void Parser::getTok() {
  SavedLoc = CurLoc;
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

Value *Parser::parseSingleStm() {
  getTok();
  if (CurTok == IF)
    writeError("Cannot parse if yet");
  else
    writeError("Cannot parse expressions yet");
  CurStatus = false;
  return nullptr;
}

BasicBlock *Parser::parseCompoundBody() {
  std::vector<Value *> StmList;

  while (CurTok != '}') {
    StmList.push_back(parseSingleStm());
    getTok();
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
  auto FcnLoc = SavedLoc;
  getTok();
  if (CurTok != '[') {
    writeError("Expected '[' to start function argument list");
    CurStatus = false;
  }
  auto ArgList = parseArgumentList();
  auto Ty = parseOptionalTypeAnnotation();
  auto FTy = FunctionType::get(Ty, K);
  BasicBlock *Body;
  if (CurTok == '{')
    Body = parseCompoundBody();
  else {
    parseSingleStm();
  }
  auto Fcn = Function::get(FcnName, FTy);
  Fcn->setSourceLocation(FcnLoc);
  return Fcn;
}

void Parser::parseToplevelForm() {
  parseFnDecl();
}

bool Parser::parse() {
  parseToplevelForm();
  return CurStatus;
}
}
