#include "rhine/IR/Type.hpp"
#include "rhine/Parse/ParseDriver.hpp"
#include "rhine/Parse/Parser.hpp"

#include <vector>

#define K Driver->Ctx

namespace rhine {
Type *Parser::parseTypeImpl(bool Optional) {
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
    auto BuildTFunction = [TFcnLoc](Type *Ty, std::vector<Type *> &TypeList,
                                    bool VariadicRest) {
      auto FTy = FunctionType::get(Ty, TypeList, VariadicRest);
      auto PTy = PointerType::get(FTy);
      FTy->setSourceLocation(TFcnLoc);
      PTy->setSourceLocation(TFcnLoc);
      return PTy;
    };
    std::vector<Type *> TypeList;
    while (auto Ty = parseTypeImpl(true)) {
      if (!getTok(ARROW)) {
        if (!getTok(')')) {
          writeError(
              "in function type of form 'Function(...)', ')' is missing");
          return nullptr;
        }
        return BuildTFunction(Ty, TypeList, false);
      }
      TypeList.push_back(Ty);
    }
    if (getTok('&'))
      if (getTok(ARROW))
        if (auto RTy = parseTypeImpl()) {
          if (!getTok(')')) {
            writeError(
                "in function type of form 'Function(...)', ')' is missing");
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

Type *Parser::parseType(bool Optional) {
  if (auto Ty = parseTypeImpl(Optional)) {
    return Ty;
  }
  return UnType::get(K);
}
}
