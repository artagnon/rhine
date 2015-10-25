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
  LastTok = CurTok;
  CurTok = Driver->Lexx->lex(&CurSema, &CurLoc);
  LastTokWasNewlineTerminated = false;
  while (CurTok == NEWLINE) {
    LastTokWasNewlineTerminated = true;
    CurTok = Driver->Lexx->lex(&CurSema, &CurLoc);
  }
}

bool Parser::getTok(int Expected) {
  auto Ret = CurTok == Expected;
  if (Ret) {
    getTok();
    CurLoc.Begin.Filename = CurLoc.End.Filename = Driver->StreamName;
  }
  return Ret;
}

void Parser::writeError(std::string ErrStr, bool Optional) {
  if (Optional) return;
  K->DiagPrinter->errorReport(CurLoc, ErrStr);
  CurStatus = false;
}

void Parser::getSemiTerm(std::string ErrFragment) {
  if (!getTok(';') && !LastTokWasNewlineTerminated && CurTok != ENDBLOCK)
    writeError("expecting ';' or newline to terminate " + ErrFragment);
}

Value *Parser::parseRtoken(bool Optional) {
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
    auto LitName = *CurSema.LiteralName;
    auto LitLoc = CurLoc;
    getTok();
    auto Ty = parseTypeAnnotation(true);
    auto Sym = UnresolvedValue::get(LitName, Ty);
    Sym->setSourceLocation(LitLoc);
    return Sym;
  }
  default:
    writeError("expecting an integer, boolean, or string literal", Optional);
  }
  return nullptr;
}

Value *Parser::parseAssignable(bool Optional) {
  if (auto Rtok = parseRtoken(Optional)) {
    Rtok->setSourceLocation(CurLoc);
    if (auto ArithOp = parseArithOp(Rtok, true))
      return ArithOp;
    return Rtok;
  }
  auto LambdaLoc = CurLoc;
  if (getTok(LAMBDA)) {
    auto ArgList = parseArgumentList(true, true);
    auto Fcn = buildFcn("lambda", ArgList, UnType::get(K), LambdaLoc);
    if (auto Block = parseBlock(ARROW, "->", {{ENDBLOCK, "END"}})) {
      Fcn->push_back(Block);
      return Fcn;
    }
    writeError("unable to parse lambda body");
    return nullptr;
  }
  writeError("expected assignable expression", Optional);
  return nullptr;
}

bool Parser::parseDollarOp(bool Optional) {
  if (!getTok('$')) {
    writeError("expected dollar ('$') operator", Optional);
    return false;
  }
  return true;
}

Value *Parser::parseRet() {
  auto RetLoc = CurLoc;
  if (!getTok(RET)) {
    writeError("expected 'ret' statement");
    return nullptr;
  }
  if (parseDollarOp(true)) {
    if (auto Stm = parseSingleStm()) {
      auto Ret = ReturnInst::get(Stm, K);
      Ret->setSourceLocation(RetLoc);
      return Ret;
    }
    writeError("expected 'ret $' to be followed by a single statement");
  }
  if (auto Lit = parseRtoken(true)) {
    getSemiTerm("return statement");
    auto Ret = ReturnInst::get(Lit, K);
    Ret->setSourceLocation(RetLoc);
    return Ret;
  }
  if (getTok('(')) {
    if (getTok(')')) {
      getSemiTerm("return statement");
      auto Ret = ReturnInst::get({}, K);
      Ret->setSourceLocation(RetLoc);
      return Ret;
    }
  }
  writeError("'ret' must be followed by an rtoken, '$', or '()'");
  return nullptr;
}

Value *Parser::parsePostLiteralName(Value *Rtok) {
  auto LitLoc = Rtok->getSourceLocation();
  if (auto Bind = parseBind(Rtok, true)) {
    getSemiTerm("bind");
    return Bind;
  }
  if (auto Mutate = parseMutate(Rtok, true)) {
    getSemiTerm("mutate");
    return Mutate;
  }
  if (auto Call = parseCall(Rtok, true)) {
    getSemiTerm("function call");
    return Call;
  }
  if (parseDollarOp(true)) {
    if (auto Stm = parseSingleStm()) {
      auto Call = CallInst::get(Rtok, {Stm});
      Call->setSourceLocation(LitLoc);
      return Call;
    }
    writeError("expected '<callee> $' to be followed by a single statement");
  }
  if (auto ArithOp = parseArithOp(Rtok, true)) {
    getSemiTerm("arithmetic op");
    return ArithOp;
  }
  writeError("expected call, assign, or arithmetic op");
  getTok();
  return nullptr;
}

Value *Parser::parseSingleStm() {
  switch (CurTok) {
  case RET: {
    if (auto Ret = parseRet())
      return Ret;
  }
  case INTEGER:
  case BOOLEAN:
  case STRING: {
    if (auto ArithOp = parseAssignable()) {
      getSemiTerm("arithmetic operation");
      return ArithOp;
    }
  }
  case LITERALNAME: {
    auto LitLoc = CurLoc;
    if (auto Rtok = parseRtoken()) {
      Rtok->setSourceLocation(LitLoc);
      return parsePostLiteralName(Rtok);
    }
  }
  case IF: {
    auto IfLoc = CurLoc;
    if (auto Expr = parseIf()) {
      Expr->setSourceLocation(IfLoc);
      return Expr;
    }
    writeError("if expression parse failed");
  }
  default:
    writeError("expecting a single statement");
    getTok();
  }
  return nullptr;
}

void Parser::parseToplevelForms() {
  getTok();
  while (auto Fcn = parseFcnDecl(true)) {
    Driver->Root.appendFunction(Fcn);
  }
  if (CurTok != END)
    writeError("expected end of file");
}

bool Parser::parse() {
  parseToplevelForms();
  return CurStatus;
}
}
