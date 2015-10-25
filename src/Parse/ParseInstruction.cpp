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

Instruction *Parser::parseAssignment(Value *Op0, bool IsMutation,
                                     bool Optional) {
  if (IsMutation && !getTok(MUTATE))
    writeError("expected '=!'", Optional);
  else if (!IsMutation && !getTok(BIND))
    writeError("expected '='", Optional);
  else {
    if (auto Rhs = parseAssignable(Optional)) {
      auto Inst = MallocInst::get(Op0->getName(), Rhs);
      Inst->setSourceLocation(Op0->getSourceLocation());
      return Inst;
    }
    writeError("rhs of assignment unparseable", Optional);
  }
  return nullptr;
}

Instruction *Parser::parseCall(Value *Callee, bool Optional) {
  auto CallLoc = Callee->getSourceLocation();
  if (auto Arg0 = parseRtoken(true)) {
    std::vector<Value *> CallArgs = {Arg0};
    while (auto Tok = parseRtoken(true))
      CallArgs.push_back(Tok);
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
  auto FalseBlock = BasicBlock::get("false", {}, K);
  if (LastTok == ELSE)
    FalseBlock = parseBlock(0, "", {{ENDBLOCK, "end"}});
  return IfInst::get(Conditional, TrueBlock, FalseBlock);
}
}
