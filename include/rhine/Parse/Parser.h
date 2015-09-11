// -*- C++ -*-
#ifndef RHINE_PARSER_H
#define RHINE_PARSER_H

#include <string>
#include "rhine/Parse/ParseDriver.h"

namespace rhine {
class BasicBlock;
class Function;
class UnresolvedValue;
class Type;
class Value;

class Parser {
public:
  enum Token {
    END,
    DEF,
    IF,
    ELSE,
    AND,
    OR,
    ARROW,
    TINT,
    TBOOL,
    TSTRING,
    TFUNCTION,
    TVOID,
    RET,
    LITERALNAME,
    INTEGER,
    BOOLEAN,
    STRING,
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
  Location SavedLoc;
  Location CurLoc;
  bool CurStatus;

  // All necessary information will be scooped out of the driver
  Parser(ParseDriver *Dri);
  virtual ~Parser();

  void getTok();
  Type *parseOptionalTypeAnnotation();
  std::vector<UnresolvedValue *> parseArgumentList();
  Value *parseSingleStm();
  BasicBlock *parseCompoundBody();
  Function *parseFnDecl();
  void parseToplevelForm();

  // The main driver; sets Driver->Root and returns success status
  bool parse();
};
}

#endif
