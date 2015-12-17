// -*- C++ -*-
#ifndef RHINE_PARSER_H
#define RHINE_PARSER_H

#include <string>
#include <map>
#include <vector>

namespace rhine {
class UnresolvedValue;
class ParseDriver;
class Instruction;
class BasicBlock;
class Function;
class Argument;
class Value;
class Type;

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
    DOBLOCK = -17,
    ENDBLOCK = -18,
    LAMBDA = -19,
    NEWLINE = -20,
    BIND = -21,
    MUTATE = -22,
  };

  class Position {
  public:
    std::string Filename;
    unsigned Line;
    unsigned Column;
    Position() : Line(1u), Column(1u) {}
    void lines(int Count = 1) {
      Column = 1u;
      Line = Line + Count;
    }
    void columns(int Count) { Column += Count; }
  };

  class Location {
  public:
    void step() { Begin = End; }
    void columns(int count = 1) { End.columns(count); }
    void lines(int count = 1) { End.lines(count); }

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
  int LastTok;
  bool LastTokWasNewlineTerminated;
  Semantic CurSema;
  Location CurLoc;
  bool CurStatus;

  /// All necessary information will be scooped out of the driver
  Parser(ParseDriver *Dri);
  virtual ~Parser();

  /// The main reader which consumes a token from the lexer, setting CurSema and
  /// CurLoc for people to construct objects.
  void getTok();

  /// Matches the current Tok against Expected, and then consumes a token on a
  /// match; on non-match, nothing is returned
  bool getTok(int Expected);

  /// The master error reporter that calls out to DiagPrinter with CurLoc and
  /// ErrStr, and sets CurStatus; does nothing if Optional is true
  void writeError(std::string ErrStr, bool Optional = false);

  /// Specialized error writer for semicolon termination
  bool getSemiTerm(std::string ErrFragment, bool Optional = false);

  /// The functions all assume that CurTok is primed for them to read, and do
  /// not getTok() at start; they do, however, make sure that CurTok is primed
  /// for the next person at the end of their operation. This works out really
  /// well for callers who "parse, but oh no, we don't handle this"

  /// Parse a type (recursive in the case of FunctionType)
  Type *parseType(bool Optional = false);

  /// Small wrapper around parseType() to consume '~' additionally
  Type *parseTypeAnnotation(bool Optional = false);

  /// Arguments are parsed along with optional type information; Parenless
  /// dictates whether the arguments can be parsed as-is without parsing '(' or
  /// ')'
  std::vector<Argument *> parseArgumentList(bool Optional = false,
                                            bool Parenless = false);

  /// Single token that can appear on the rhs of '='
  Value *parseRtoken(bool Optional = false);

  /// Anything that can come on the right of '='; this is a superset of Rtoken
  Value *parseAssignable(bool Optional = false);

  /// Small helper to determine whether we're looking at a '$' operator
  bool parseDollarOp(bool Optional = false);

  /// Parse the stuff following a 'ret'
  Instruction *parseRet();

  /// Parse the stuff following a literal name in a statement
  Instruction *parsePostLiteralName(Value *Rtok);

  /// Assuming the lhs has already been parsed (passed in as the first
  /// argument), look at '=' and parse the rhs to build a full MallocInst to
  /// return
  Instruction *parseBind(Value *Op0, bool Optional = false);

  /// Assuming the lhs has already been parsed (passed in as the first
  /// argument), look at ':=' and parse the rhs to build a
  /// UnresolvedValue-StoreInst to return
  Instruction *parseMutate(Value *Op0, bool Optional = false);

  /// Assuming the lhs has already been parsed (passed in as first argument),
  /// parse the appropriate arithmetic operator and build the full Instruction
  /// to return
  Instruction *parseArithOp(Value *Op0, bool Optional = false);

  /// Callee has already been parsed and is passed in as the first argument
  Instruction *parseCall(Value *Callee, bool Optional = false);

  /// Parse an 'if' statement: possible else clause, but no else-if
  Instruction *parseIf();

  /// A statement is not branch, and ends with a semicolon
  Value *parseSingleStmt();

  /// Small helper for parseBlock
  bool matchesAnyTokenPair(std::map<int, std::string> &TokenPairs);

  /// A block (function body, if statemnet, lambda etc) that's delimited by
  /// StartToken and EndToken
  BasicBlock *parseBlock(int StartToken, std::string StartTokenStr,
                         std::map<int, std::string> EndTokens);

  /// Function builder-helper; mainly, extracts types from the ArgList, creates
  /// a FunctionType using that and the return type, and finally builds a
  /// function with that type and name FcnName
  Function *buildFcn(std::string FcnName, std::vector<Argument *> &ArgList,
                     Type *ReturnType, Location &FcnLoc);

  /// DEF <NAME> ['(' [ARGUMENTS ...] ')']
  Function *parseFcnDecl(bool Optional = false);

  /// Room for global string and other structures
  void parseToplevelForms();

  /// The main driver; sets Driver->Root and returns success status
  bool parse();
};
}

#endif
