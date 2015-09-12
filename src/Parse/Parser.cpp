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

bool Parser::getTok(int expected) {
  auto Ret = CurTok == expected;
  CurTok = Driver->Lexx->lex(&CurSema, &CurLoc);
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

Type *Parser::parseOptionalTypeAnnotation() {
  if (CurTok == '~') {
    getTok();
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
      getTok();
      if (!getTok('('))
        writeError("in function type of form Fn(...), '(' is missing");
      writeError("cannot parse function types yet");
      while (CurTok != ')')
        getTok();
      getTok();
    }
    case TVOID: {
      auto Ty = VoidType::get(K);
      Ty->setSourceLocation(CurLoc);
      return Ty;
    }
    default:
      writeError("unrecognized type name");
    }
  }
  return UnType::get(K);
}

std::vector<Argument *> Parser::parseArgumentList() {
  std::vector<Argument *> ArgumentList;
  while (CurTok != ']') {
    if (CurTok != LITERALNAME)
      writeError("expected argument name");
    auto Name = *CurSema.LiteralName;
    getTok();
    auto Ty = parseOptionalTypeAnnotation();
    ArgumentList.push_back(Argument::get(Name, Ty));
  }
  getTok();
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
    auto RawName = *CurSema.LiteralName;
    auto SymLoc = CurLoc;
    getTok();
    auto Ty = parseOptionalTypeAnnotation();
    auto Sym = UnresolvedValue::get(RawName, Ty);
    Sym->setSourceLocation(SymLoc);
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
  if (auto Rhs = parseAssignable(Optional)) {
    auto Inst = MallocInst::get(Op0->getName(), Rhs);
    Inst->setSourceLocation(Op0->getSourceLocation());
    return Inst;
  }
  writeError("rhs of assignment unparseable", Optional);
  return nullptr;
}

Instruction *Parser::parseCall(Value *Callee, bool Optional) {
  writeError("cannot parse calls yet");
  return nullptr;
}

Value *Parser::parseSingleStm() {
  if (CurTok == IF)
    writeError("cannot parse if yet");
  else {
    switch (CurTok) {
    case RET: {
      auto RetLoc = CurLoc;
      getTok();
      auto Lit = parseRtoken();
      if (!getTok(';'))
        writeError("expecting ';' to terminate return statement");
      auto Ret = ReturnInst::get(Lit, K);
      Ret->setSourceLocation(RetLoc);
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
      if (auto Rtok = parseRtoken()) {
        if (auto Assign = parseAssignment(Rtok, true)) {
          getSemiTerm("assignment");
          return Assign;
        }
        if (auto Call = parseCall(Rtok, true)) {
          getSemiTerm("function call");
          return Call;
        }
      }
      if (auto ArithOp = parseAssignable(true)) {
        getSemiTerm("statement");
        return ArithOp;
      }
      writeError("expected call, assign, or arithmetic op");
    }
    default:
      writeError("expecting a single statement");
    }
  }
  return nullptr;
}

BasicBlock *Parser::parseCompoundBody() {
  std::vector<Value *> StmList;

  while (CurTok != '}' && CurTok != END)
    StmList.push_back(parseSingleStm());
  if (CurTok == END) {
    writeError("Dangling compound body");
  }
  return BasicBlock::get("entry", StmList, K);
}

Function *Parser::parseFnDecl() {
  if (!getTok(DEF))
    writeError("expected 'def', to begin function definition");
  if (CurTok != LITERALNAME)
    writeError("expected function name");
  auto FcnName = *CurSema.LiteralName;
  auto FcnLoc = CurLoc;
  getTok();
  if (!getTok('['))
    writeError("expected '[' to start function argument list");
  auto ArgList = parseArgumentList();
  auto Ty = parseOptionalTypeAnnotation();
  std::vector<Type *> ATys;
  for (auto Sym : ArgList)
    ATys.push_back(Sym->getType());
  auto FTy = FunctionType::get(Ty, ATys, false);
  auto Fcn = Function::get(FcnName, FTy);
  Fcn->setArguments(ArgList);
  Fcn->setSourceLocation(FcnLoc);

  if (CurTok == '{') {
    getTok();
    Fcn->push_back(parseCompoundBody());
  } else {
    Fcn->push_back(BasicBlock::get("entry", { parseSingleStm() }, K));
    if (CurTok != END)
      writeError("trailing garbage");
  }
  return Fcn;
}

void Parser::parseToplevelForms() {
  getTok();
  Driver->Root.appendFunction(parseFnDecl());
}

bool Parser::parse() {
  parseToplevelForms();
  return CurStatus;
}
}
