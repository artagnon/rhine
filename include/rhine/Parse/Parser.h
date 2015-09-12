// -*- C++ -*-
#ifndef RHINE_PARSER_H
#define RHINE_PARSER_H

#include <string>
#include "rhine/Parse/ParseDriver.h"

namespace rhine {
class BasicBlock;
class Function;
class UnresolvedValue;
class Argument;
class Instruction;
class Type;
class Value;

class Parser {
public:
  enum Token {
    END = 0,
    DEF = -1,
    IF = -2,
    ELSE = -3,
    AND = -4,
    OR = -5,
    ARROW = -6,
    TINT = -7,
    TBOOL = -8,
    TSTRING = -9,
    TFUNCTION = -10,
    TVOID = -11,
    RET = -12,
    LITERALNAME = -13,
    INTEGER = -14,
    BOOLEAN = -15,
    STRING = -16,
  };

  class Position {
  public:
    std::string *Filename;
    unsigned Line;
    unsigned Column;
    Position() :
        Line(1u), Column(1u) {
      static std::string Dummy;
      Filename = &Dummy;
    }
    void lines (int Count = 1)
    {
      Column = 1u;
      Line = Line + Count;
    }
    /// (Column related) Advance to the COUNT next columns.
    void columns (int Count)
    {
      Column += Count;
    }
  };

  class Location {
  public:
    /// Reset initial Location to final Location.
    void step ()
    {
      Begin = End;
    }

    /// Extend the current Location to the COUNT next columns.
    void columns (int count = 1)
    {
      End.columns(count);
    }

    /// Extend the current Location to the COUNT next lines.
    void lines (int count = 1)
    {
      End.lines(count);
    }

  public:
    Position Begin;
    Position End;
  };

  union Semantic {
    std::string *LiteralName;
    class ConstantInt *Integer;
    class ConstantBool *Boolean;
    class ConstantFloat *Float;
    class GlobalString *String;
  };

  ParseDriver *Driver;

  // Current state
  int CurTok;
  Semantic CurSema;
  Location CurLoc;
  bool CurStatus;

  /// All necessary information will be scooped out of the driver
  Parser(ParseDriver *Dri);
  virtual ~Parser();

  /// The main reader which consumes a token from the lexer, setting CurSema and
  /// CurLoc for people to construct objects.
  void getTok();
  bool getTok(int Expected);

  /// The master error reporter that calls out to DiagPrinter with CurLoc and
  /// ErrStr, and sets CurStatus; does nothing if Optional is true
  void writeError(std::string ErrStr, bool Optional = false);

  /// Specialized error writer for semicolon termination
  void getSemiTerm(std::string ErrFragment);

  /// The functions all assume that CurTok is primed for them to read, and do
  /// not getTok() at start; they do, however, make sure that CurTok is primed
  /// for the next person at the end of their operation. This works out really
  /// well for callers who "parse, but oh no, we don't handle this".
  Type *parseOptionalTypeAnnotation();
  std::vector<Argument *> parseArgumentList();
  Value *parseRtoken(bool Optional = false);
  Value *parseAssignable(bool Optional = false);
  Instruction *parseAssignment(Value *Op0, bool Optional = false);
  Instruction *parseArithOp(Value *Op0, bool Optional = false);
  Instruction *parseCall(Value *Callee, bool Optional = false);
  Value *parseSingleStm();
  BasicBlock *parseCompoundBody();
  Function *parseFnDecl();
  void parseToplevelForms();

  /// The main driver; sets Driver->Root and returns success status
  bool parse();
};
}

#endif
