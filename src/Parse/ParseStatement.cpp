#include "rhine/Parse/ParseDriver.h"
#include "rhine/Parse/Parser.h"

#include "rhine/IR/Instruction.h"
#include "rhine/IR/GlobalValue.h"
#include "rhine/IR/Function.h"
#include "rhine/IR/Context.h"
#include "rhine/IR/Value.h"

#define K Driver->Ctx

namespace rhine {
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
  auto IfLoc = CurLoc;
  if (auto Expr = parseIf()) {
    Expr->setSourceLocation(IfLoc);
    return Expr;
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

Instruction *Parser::parsePostLiteralName(Value *Rtok) {
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
    if (auto Stmt = parseSingleStmt()) {
      auto Call = CallInst::get(Rtok, {Stmt});
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

Value *Parser::parseSingleStmt() {
  switch (CurTok) {
  case RET: {
    if (auto Ret = parseRet())
      return Ret;
  }
  case INTEGER:
  case BOOLEAN:
  case STRING: {
    if (auto AssignableExpr = parseAssignable()) {
      getSemiTerm("assignable expression");
      return AssignableExpr;
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
}
