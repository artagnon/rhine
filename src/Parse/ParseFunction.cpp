#include "rhine/IR/BasicBlock.hpp"
#include "rhine/IR/Constant.hpp"
#include "rhine/IR/Function.hpp"
#include "rhine/IR/GlobalValue.hpp"
#include "rhine/IR/Instruction.hpp"
#include "rhine/IR/Type.hpp"
#include "rhine/IR/UnresolvedValue.hpp"
#include "rhine/IR/Value.hpp"
#include "rhine/Parse/ParseDriver.hpp"
#include "rhine/Parse/Parser.hpp"

#include <vector>

namespace rhine {
std::vector<Argument *> Parser::parseArgumentList(bool Optional,
                                                  bool Parenless) {
  if (!Parenless && getTok(VOID)) {
    return {};
  }
  if (!Parenless && !getTok('(')) {
    writeError("expected '(' to begin argument list", Optional);
    return {};
  }
  std::vector<Argument *> ArgumentList;
  while (CurTok != ')') {
    auto ArgLoc = CurLoc;
    auto ArgSema = CurSema;
    if (!getTok(LITERALNAME)) {
      if (Parenless)
        break;
      writeError("expected argument name");
      return {};
    }
    if (auto Ty = parseTypeAnnotation(true)) {
      auto Arg = Argument::get(*ArgSema.LiteralName, Ty);
      Arg->setSourceLocation(ArgLoc);
      ArgumentList.push_back(Arg);
      if (!getTok(','))
        break;
    } else {
      writeError("malformed argument type specified");
      return {};
    }
  }
  if (!Parenless && !getTok(')')) {
    writeError("expected ')' to end function argument list, or ',' to separate "
               "arguments");
    return {};
  }
  return ArgumentList;
}

Function *Parser::parseFcnDecl(bool Optional) {
  if (!getTok(DEF)) {
    writeError("expected 'def', to begin function definition", Optional);
    return nullptr;
  }
  auto FcnLoc = CurLoc;
  auto FcnName = *CurSema.LiteralName;
  if (!getTok(LITERALNAME)) {
    writeError("expected function name");
    return nullptr;
  }
  auto ArgList = parseArgumentList(true);
  auto ReturnTy = parseTypeAnnotation(true);
  auto Fcn = buildFcn(FcnName, ArgList, ReturnTy, FcnLoc);
  delete CurSema.LiteralName;
  if (auto Block = parseBlock({DOBLOCK, "do"}, {{ENDBLOCK, "end"}}))
    Fcn->push_back(Block);
  else {
    writeError("unexpected empty block");
    return nullptr;
  }
  return Fcn;
}
}
