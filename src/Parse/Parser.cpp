#include "rhine/Parse/Lexer.h"
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

#include <vector>

#define K Driver->Ctx

namespace rhine {
Parser::Parser(ParseDriver *Dri) : Driver(Dri), CurStatus(true) {}

Parser::~Parser() {}

void Parser::getTok() {
  CurTok = Driver->Lexx->lex(&CurSema, &CurLoc);
}

bool Parser::getTok(int Expected) {
  auto Ret = CurTok == Expected;
  if (Ret) {
    CurTok = Driver->Lexx->lex(&CurSema, &CurLoc);
    CurLoc.Begin.Filename = CurLoc.End.Filename = Driver->StreamName;
  }

  return Ret;
}

void Parser::writeError(std::string ErrStr, bool Optional) {
  if (Optional) return;
  K->DiagPrinter->errorReport(CurLoc, ErrStr);
  CurStatus = false;
}

void Parser::getSemiTerm(std::string ErrFragment) {
  if (!getTok(';'))
    writeError("expecting ';' to terminate " + ErrFragment);
}

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

std::vector<Argument *> Parser::parseArgumentList(bool Optional,
                                                  bool Parenless) {
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
    } else {
      writeError("malformed argument type specified");
      return {};
    }
  }
  if (!Parenless && !getTok(')')) {
    writeError("expected ')' to end function argument list");
    return {};
  }
  return ArgumentList;
}

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
    if (auto Block = parseBlock(/* ArrowStarter = */ true)) {
      Fcn->push_back(Block);
      return Fcn;
    }
    writeError("unable to parse lambda body");
    return nullptr;
  }
  writeError("expected assignable expression", Optional);
  return nullptr;
}

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

Instruction *Parser::parseAssignment(Value *Op0, bool Optional) {
  if (!getTok('='))
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
    std::vector<Value *> CallArgs = { Arg0 };
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

bool Parser::parseDollarOp(bool Optional) {
  if (!getTok('$')) {
    writeError("expected dollar ('$') operator", Optional);
    return false;
  }
  return true;
}

Value *Parser::parseRet() {
  auto RetLoc = CurLoc;
  if (!getTok(RET)) {
    writeError("expected 'ret' statement");
    return nullptr;
  }
  if (parseDollarOp(true)) {
    if (auto Stm = parseSingleStm()) {
      auto Ret = ReturnInst::get(Stm, K);
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

Value *Parser::parsePostLiteralName(Value *Rtok) {
  auto LitLoc = Rtok->getSourceLocation();
  if (auto Assign = parseAssignment(Rtok, true)) {
    getSemiTerm("assignment");
    return Assign;
  }
  if (auto Call = parseCall(Rtok, true)) {
    getSemiTerm("function call");
    return Call;
  }
  if (parseDollarOp(true)) {
    if (auto Stm = parseSingleStm()) {
      auto Call = CallInst::get(Rtok, {Stm});
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
  return nullptr;
}

Value *Parser::parseSingleStm() {
  switch (CurTok) {
  case RET: {
    if (auto Ret = parseRet())
      return Ret;
  }
  case INTEGER:
  case BOOLEAN:
  case STRING: {
    if (auto ArithOp = parseAssignable()) {
      getSemiTerm("arithmetic operation");
      return ArithOp;
    }
  }
  case LITERALNAME: {
    auto LitLoc = CurLoc;
    if (auto Rtok = parseRtoken()) {
      Rtok->setSourceLocation(LitLoc);
      return parsePostLiteralName(Rtok);
    }
  }
  default:
    writeError("expecting a single statement");
    getTok();
  }
  return nullptr;
}

BasicBlock *Parser::parseBlock(bool ArrowStarter) {
  std::vector<Value *> StmList;
  if (ArrowStarter) {
    if (!getTok(ARROW)) {
      writeError("expected '->' to start block");
      return nullptr;
    }
  } else {
    if (!getTok(DOBLOCK)) {
      writeError("expected 'do' to start block");
      return nullptr;
    }
  }

  while (CurTok != ENDBLOCK && CurTok != END)
    if (auto Stm = parseSingleStm())
      StmList.push_back(Stm);

  if (!getTok(ENDBLOCK)) {
    writeError("expected 'end' to end block");
    return nullptr;
  }
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
  auto OptionalTypeAnnLoc = CurLoc;
  auto Fcn = buildFcn(FcnName, ArgList, UnType::get(K), FcnLoc);
  if (auto Block = parseBlock())
    Fcn->push_back(Block);
  else {
    writeError("unexpected empty block");
    return nullptr;
  }
  return Fcn;
}

void Parser::parseToplevelForms() {
  getTok();
  while (auto Fcn = parseFcnDecl(true)) {
    Driver->Root.appendFunction(Fcn);
  }
  if (CurTok != END)
    writeError("expected end of file");
}

bool Parser::parse() {
  parseToplevelForms();
  return CurStatus;
}
}
