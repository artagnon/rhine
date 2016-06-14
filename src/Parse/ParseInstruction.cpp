#include "rhine/IR/BasicBlock.hpp"
#include "rhine/IR/Constant.hpp"
#include "rhine/IR/GlobalValue.hpp"
#include "rhine/IR/Instruction.hpp"
#include "rhine/IR/Tensor.hpp"
#include "rhine/IR/Type.hpp"
#include "rhine/IR/UnresolvedValue.hpp"
#include "rhine/IR/Value.hpp"
#include "rhine/Parse/ParseDriver.hpp"
#include "rhine/Parse/Parser.hpp"

#define K Driver->Ctx

namespace rhine {
BinaryArithInst *Parser::parseArithOp(Value *Op0, bool Optional) {
  RTValue InstructionSelector;
  switch (CurTok) {
  case '+':
    InstructionSelector = RT_AddInst;
    break;
  case '-':
    InstructionSelector = RT_SubInst;
    break;
  case '*':
    InstructionSelector = RT_MulInst;
    break;
  case '/':
    InstructionSelector = RT_DivInst;
    break;
  default:
    writeError("expected '+' or '-'", Optional);
    return nullptr;
  }
  getTok();
  auto Op1 = parseRtoken();
  return BinaryArithInst::get(InstructionSelector, Op0, Op1);
}

AbstractBindInst *Parser::parseBind(Value *Op0, bool Optional) {
  if (!getTok(BIND))
    writeError("expected '='", Optional);
  else {
    if (auto Rhs = parseAssignable(Optional)) {
      AbstractBindInst *Inst;
      if (isa<Tensor>(Rhs))
        Inst = BindInst::get(Op0->getName(), Rhs);
      else
        Inst = MallocInst::get(Op0->getName(), Rhs);
      Inst->setSourceLocation(Op0->sourceLocation());
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
      Inst->setSourceLocation(Op0->sourceLocation());
      return Inst;
    }
    writeError("rhs of mutate unparseable", Optional);
  }
  return nullptr;
}

bool Parser::parseCallArgs(std::vector<Value *> &CallArgs) {
  auto Arg0 = parseRtoken(true);
  if (!Arg0) {
    if (getTok(VOID)) {
      CallArgs = {};
      return true;
    }
    return false;
  }
  CallArgs = {Arg0};
  while (!LastTokWasNewlineTerminated) {
    if (auto Tok = parseRtoken(true))
      CallArgs.push_back(Tok);
    else
      break;
  }
  return true;
}

CallInst *Parser::parseCall(Value *Callee, bool Optional) {
  auto CallLoc = Callee->sourceLocation();
  std::vector<Value *> CallArgs;
  if (parseCallArgs(CallArgs)) {
    auto Inst = CallInst::get(Callee, CallArgs);
    Inst->setSourceLocation(CallLoc);
    return Inst;
  }
  writeError("expecting first call argument, '$' or '()'", Optional);
  return nullptr;
}

IfInst *Parser::parseIf() {
  auto IfLoc = CurLoc;
  if (!getTok(IF)) {
    writeError("expected 'if' expression");
    return nullptr;
  }
  auto Conditional = parseRtoken();
  auto TrueBlock =
      parseBlock({DOBLOCK, "do"}, {{ELSE, "else"}, {ENDBLOCK, "end"}});
  if (!TrueBlock)
    return nullptr;
  auto FalseBlock = BasicBlock::get("false", {}, K);
  if (LastTok == ELSE)
    FalseBlock = parseBlock({0, ""}, {{ENDBLOCK, "end"}});
  auto Inst = IfInst::get(Conditional, TrueBlock, FalseBlock);
  Inst->setSourceLocation(IfLoc);
  return Inst;
}

IndexingInst *Parser::parseIndexingInst(Value *Sym, bool Optional) {
  if (CurTok != '[') {
    if (!Optional) {
      writeError("Expected an indexing expression of the form [...]");
    }
    return nullptr;
  }
  std::vector<Value *> Idxes;
  while (getTok('[')) {
    Idxes.push_back(parseRtoken());
    if (!getTok(']')) {
      writeError("Expected ']' to match '[' of indexing expression");
    }
  }
  auto Inst = IndexingInst::get(Sym, Idxes);
  Inst->setSourceLocation(Sym->sourceLocation());
  return Inst;
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
  if (getTok(VOID)) {
    getSemiTerm("return statement");
    auto Ret = ReturnInst::get({}, K);
    Ret->setSourceLocation(RetLoc);
    return Ret;
  }
  writeError("'ret' must be followed by an rtoken, '$', or '()'");
  return nullptr;
}
}
