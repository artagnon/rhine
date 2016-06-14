#include "rhine/Parse/ParseDriver.hpp"
#include "rhine/Parse/Parser.hpp"

#include "rhine/IR/Context.hpp"
#include "rhine/IR/Function.hpp"
#include "rhine/IR/GlobalValue.hpp"
#include "rhine/IR/Instruction.hpp"
#include "rhine/IR/Tensor.hpp"
#include "rhine/IR/Value.hpp"

#define K Driver->Ctx

namespace rhine {
template <typename T> T *Parser::parseConstant() {
  auto Const = *(T **)&CurSema;
  Const->setSourceLocation(CurLoc);
  getTok();
  return Const;
}

std::pair<ValueVector, std::vector<size_t>> Parser::parseTensorND() {
  /// We can see one of two things:
  /// Value *: which means that we're at the end, parseTensor1D
  /// '{': which means there's another dimension; recursive call
  ValueVector ValVec;
  std::vector<size_t> Shape;
  auto NElts = 0;
  if (getTok('{')) {
    do {
      NElts++;
      if (auto Tok = parseRtoken(true, true)) {
        ValVec.push_back(Tok);
      } else {
        ValueVector ThisValVec;
        std::tie(ThisValVec, Shape) = parseTensorND();
        ValVec.insert(ValVec.end(), ThisValVec.begin(), ThisValVec.end());
      }
    } while (getTok(','));
    if (!getTok('}')) {
      writeError("Expected '}' to end Tensor dimension");
      return {};
    }
  }
  Shape.push_back(NElts);
  return std::make_pair(ValVec, Shape);
}

Tensor *Parser::parseTensor(bool Optional) {
  ValueVector TensorValues;
  std::vector<size_t> Shape;
  std::tie(TensorValues, Shape) = parseTensorND();
  std::reverse(Shape.begin(), Shape.end());
  return Tensor::get(Shape, TensorValues, K);
}

Value *Parser::parseRtoken(bool Optional, bool ParsingTensor) {
  switch (CurTok) {
  case INTEGER:
    return parseConstant<ConstantInt>();
  case BOOLEAN:
    return parseConstant<ConstantBool>();
  case STRING:
    return parseConstant<GlobalString>();
  case '{':
    if (ParsingTensor) {
      return nullptr;
    }
    return parseTensor();
  case LITERALNAME: {
    auto LitName = *CurSema.LiteralName;
    auto LitLoc = CurLoc;
    getTok();
    auto Ty = parseType(true);
    auto Sym = UnresolvedValue::get(LitName, Ty);
    Sym->setSourceLocation(LitLoc);
    if (auto Inst = parseIndexingInst(Sym, true)) {
      return Inst;
    }
    return Sym;
  }
  default:
    writeError("expecting a literal, symbol, or indexing expression", Optional);
  }
  return nullptr;
}

Function *Parser::parseLambda(bool Optional) {
  auto LambdaLoc = CurLoc;
  if (getTok(LAMBDA)) {
    auto ArgList = parseArgumentList(true, true);
    auto Fcn = buildFcn("lambda", ArgList, UnType::get(K), LambdaLoc);
    if (auto Block = parseBlock({ARROW, "->"}, {{ENDBLOCK, "END"}})) {
      Fcn->push_back(Block);
      return Fcn;
    }
    writeError("unable to parse lambda body");
    return nullptr;
  }
  writeError("expected lambda but found " + std::to_string(CurTok), Optional);
  return nullptr;
}

Value *Parser::parseAssignable(bool Optional) {
  if (auto Rtok = parseRtoken(Optional)) {
    Rtok->setSourceLocation(CurLoc);
    if (auto ArithOp = parseArithOp(Rtok, true))
      return ArithOp;
    return Rtok;
  }
  if (auto Lamb = parseLambda(true))
    return Lamb;
  if (auto Expr = parseIf())
    return Expr;
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

Value *Parser::parsePostLiteralName(Value *Rtok) {
  auto LitLoc = Rtok->sourceLocation();
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
  /// All matches failed; it's probably an UnresolvedValue that will later be
  /// wrapped in a TerminatorInst.
  return Rtok;
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
