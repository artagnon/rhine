#include "rhine/Parse/Parser.hpp"
#include "rhine/Parse/ParseDriver.hpp"
#include "rhine/IR/UnresolvedValue.hpp"
#include "rhine/IR/GlobalValue.hpp"
#include "rhine/IR/Instruction.hpp"
#include "rhine/IR/BasicBlock.hpp"
#include "rhine/IR/Constant.hpp"
#include "rhine/IR/Value.hpp"
#include "rhine/IR/Type.hpp"

#define K Driver->Ctx

namespace rhine {
Instruction *Parser::parseArithOp(Value *Op0, bool Optional) {
  switch (CurTok) {
  case '+': {
    getTok();
    auto Op1 = parseRtoken();
    return AddInst::get(Op0, Op1);
  }
  case '-': {
    getTok();
    auto Op1 = parseRtoken();
    return SubInst::get(Op0, Op1);
  }
  default:
    writeError("expected '+' or '-'", Optional);
  }
  return nullptr;
}

Instruction *Parser::parseBind(Value *Op0, bool Optional) {
  if (!getTok(BIND))
    writeError("expected '='", Optional);
  else {
    if (auto Rhs = parseAssignable(Optional)) {
      auto Inst = MallocInst::get(Op0->getName(), Rhs);
      Inst->setSourceLocation(Op0->getSourceLocation());
      return Inst;
    }
    writeError("rhs of bind unparseable", Optional);
  }
  return nullptr;
}

Instruction *Parser::parseMutate(Value *Op0, bool Optional) {
  if (!getTok(MUTATE))
    writeError("expected ':='", Optional);
  else {
    if (auto Rhs = parseAssignable(Optional)) {
      auto UnRes = UnresolvedValue::get(Op0->getName(), Rhs->getType());
      auto Inst = StoreInst::get(UnRes, Rhs);
      Inst->setSourceLocation(Op0->getSourceLocation());
      return Inst;
    }
    writeError("rhs of mutate unparseable", Optional);
  }
  return nullptr;
}

Instruction *Parser::parseCall(Value *Callee, bool Optional) {
  auto CallLoc = Callee->getSourceLocation();
  if (auto Arg0 = parseRtoken(true)) {
    std::vector<Value *> CallArgs = {Arg0};
    while (!LastTokWasNewlineTerminated) {
      if (auto Tok = parseRtoken(true))
        CallArgs.push_back(Tok);
      else
        break;
    }
    auto Inst = CallInst::get(Callee, CallArgs);
    Inst->setSourceLocation(CallLoc);
    return Inst;
  }
  if (getTok('(')) {
    if (getTok(')')) {
      auto Inst = CallInst::get(Callee, {});
      Inst->setSourceLocation(CallLoc);
      return Inst;
    }
    writeError("expecting '()'");
    return nullptr;
  }
  writeError("expecting first call argument or '()' for Void", Optional);
  return nullptr;
}

Instruction *Parser::parseIf() {
  if (!getTok(IF)) {
    writeError("expected 'if' expression");
    return nullptr;
  }
  auto Conditional = parseRtoken();
  auto TrueBlock =
    parseBlock(DOBLOCK, "do", {{ELSE, "else"}, {ENDBLOCK, "end"}});
  if (!TrueBlock)
    return nullptr;
  auto FalseBlock = BasicBlock::get("false", {}, K);
  if (LastTok == ELSE)
    FalseBlock = parseBlock(0, "", {{ENDBLOCK, "end"}});
  return IfInst::get(Conditional, TrueBlock, FalseBlock);
}

Instruction *Parser::parseRet() {
  auto RetLoc = CurLoc;
  if (!getTok(RET)) {
    writeError("expected 'ret' statement");
    return nullptr;
  }
  if (parseDollarOp(true)) {
    if (auto Stmt = parseSingleStmt()) {
      auto Ret = ReturnInst::get(Stmt, K);
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
}
