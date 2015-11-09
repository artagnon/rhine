#include "rhine/Parse/Parser.h"
#include "rhine/IR/BasicBlock.h"
#include "rhine/IR/Instruction.h"
#include "rhine/IR/Constant.h"
#include "rhine/IR/Type.h"

#include <vector>

#define K Driver->Ctx

namespace rhine {
bool Parser::matchesAnyTokenPair(std::map<int, std::string> &TokenPairs) {
  for (auto TokPair : TokenPairs)
    if (CurTok == TokPair.first)
      return true;
  return false;
}

BasicBlock *Parser::parseBlock(int StartToken,
                               std::string StartTokenStr,
                               std::map<int, std::string> EndTokens) {
  std::vector<Instruction *> StmList;

  if (StartToken && !getTok(StartToken)) {
    writeError("expected '" + StartTokenStr + "' to start block");
    return nullptr;
  }

  while (!matchesAnyTokenPair(EndTokens) && CurTok != END) {
    if (auto Stm = parseSingleStm()) {
      if (auto Inst = dyn_cast<Instruction>(Stm))
        StmList.push_back(Inst);
      else
        writeError("expected instruction");
    }
  }

  if (!EndTokens.count(CurTok)) {
    std::string ErrStr;
    for (auto Tok : EndTokens)
      ErrStr += "'" + Tok.second + "'";
    if (EndTokens.size() > 1)
      writeError("expected one of " + ErrStr + " to end block");
    else
      writeError("expected " + ErrStr + " to end block");
    return nullptr;
  }
  getTok();
  return BasicBlock::get("entry", StmList, K);
}

Function *Parser::buildFcn(std::string FcnName,
                           std::vector<Argument *> &ArgList,
                           Type *ReturnType, Location &FcnLoc) {
  std::vector<Type *> ATys;
    for (auto Sym : ArgList)
    ATys.push_back(Sym->getType());
  auto FTy = FunctionType::get(ReturnType, ATys, false);
  auto Fcn = Function::get(FcnName, FTy);
  Fcn->setArguments(ArgList);
  Fcn->setSourceLocation(FcnLoc);
  return Fcn;
}
}
