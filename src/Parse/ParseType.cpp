#include "rhine/Parse/Parser.h"
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
Type *Parser::parseType(bool Optional) {
  switch (CurTok) {
  case TINT: {
    auto Ty = IntegerType::get(32, K);
    Ty->setSourceLocation(CurLoc);
    getTok();
    return Ty;
  }
  case TBOOL: {
    auto Ty = BoolType::get(K);
    Ty->setSourceLocation(CurLoc);
    getTok();
    return Ty;
  }
  case TSTRING: {
    auto Ty = StringType::get(K);
    Ty->setSourceLocation(CurLoc);
    getTok();
    return Ty;
  }
  case TFUNCTION: {
    auto TFcnLoc = CurLoc;
    getTok();
    if (!getTok('(')) {
      writeError("in function type of form 'Function(...)', '(' is missing");
      return nullptr;
    }
    auto BuildTFunction = [TFcnLoc] (Type *Ty,
                                     std::vector<Type *> &TypeList,
                                     bool VariadicRest) {
      auto FTy = FunctionType::get(Ty, TypeList, VariadicRest);
      auto PTy = PointerType::get(FTy);
      FTy->setSourceLocation(TFcnLoc);
      PTy->setSourceLocation(TFcnLoc);
      return PTy;
    };
    std::vector<Type *> TypeList;
    while (auto Ty = parseType(true)) {
      if (!getTok(ARROW)) {
        if (!getTok(')')) {
          writeError("in function type of form 'Function(...)', ')' is missing");
          return nullptr;
        }
        return BuildTFunction(Ty, TypeList, false);
      }
      TypeList.push_back(Ty);
    }
    if (getTok('&'))
      if (getTok(ARROW))
        if (auto RTy = parseType()) {
          if (!getTok(')')) {
            writeError("in function type of form 'Function(...)', ')' is missing");
            return nullptr;
          }
          return BuildTFunction(RTy, TypeList, true);
        }
    writeError("dangling function type specifier");
    return nullptr;
  }
  case TVOID: {
    auto Ty = VoidType::get(K);
    Ty->setSourceLocation(CurLoc);
    getTok();
    return Ty;
  }
  default:
    writeError("unrecognized type name", Optional);
  }
  return nullptr;
}

Type *Parser::parseTypeAnnotation(bool Optional) {
  if (getTok('~'))
    return parseType();
  writeError("unable to parse type annotation", Optional);
  return UnType::get(K);
}
}
