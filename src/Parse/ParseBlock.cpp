#include "rhine/IR/BasicBlock.hpp"
#include "rhine/IR/Constant.hpp"
#include "rhine/IR/Function.hpp"
#include "rhine/IR/Instruction.hpp"
#include "rhine/IR/Type.hpp"
#include "rhine/Parse/ParseDriver.hpp"
#include "rhine/Parse/Parser.hpp"

#include <algorithm>
#include <vector>

#define K Driver->Ctx

namespace rhine {
bool Parser::matchesAnyTokenPair(std::map<int, std::string> &TokenPairs) {
  for (auto TokPair : TokenPairs)
    if (CurTok == TokPair.first)
      return true;
  return false;
}

void extractInstructions(Instruction *Top,
                         std::vector<Instruction *> &Accumulate) {
  Accumulate.push_back(Top);
  for (auto Op : Top->operands()) {
    Value *Val = Op;
    if (auto Sub = dyn_cast<Instruction>(Val))
      extractInstructions(Sub, Accumulate);
  }
}

void extractInstructions(Value *Stmt, std::vector<Instruction *> &Accumulate) {
  if (auto Inst = dyn_cast<Instruction>(Stmt)) {
    std::vector<Instruction *> Pieces;
    extractInstructions(Inst, Pieces);
    for (auto It = Pieces.rbegin(); It != Pieces.rend(); ++It)
      Accumulate.push_back(*It);
  } else
    /// Dangling values at the end of blocks are tidied into TerminatorInst.
    Accumulate.push_back(TerminatorInst::get(Stmt));
}

BasicBlock *Parser::parseBlock(std::pair<int, std::string> StartToken,
                               std::map<int, std::string> EndTokens) {
  std::vector<Instruction *> InstList;

  if (std::get<int>(StartToken) && !getTok(std::get<int>(StartToken))) {
    writeError("expected '" + StartToken.second + "' to start block");
    return nullptr;
  }

  while (!matchesAnyTokenPair(EndTokens) && CurTok != END) {
    if (auto Stmt = parseSingleStmt()) {
      extractInstructions(Stmt, InstList);
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
  return BasicBlock::get("entry", InstList, K);
}

Function *Parser::buildFcn(std::string FcnName,
                           std::vector<Argument *> &ArgList, Type *ReturnType,
                           Location &FcnLoc) {
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
