#include "rhine/Util/TestUtil.h"
#include "gtest/gtest.h"

using namespace rhine;

TEST(Parse, BareDefun)
{
  std::string SourcePrg = "def foo";
  std::string ExpectedErr = "string stream:1:8: error: expected 'do' to start block";
  EXPECT_COMPILE_DEATH(SourcePrg, ExpectedErr);
}

TEST(Parse, BareDanglingDo)
{
  std::string SourcePrg = "def foo do";
  std::string ExpectedErr = "string stream:1:11: error: expected 'end' to end block";
  EXPECT_COMPILE_DEATH(SourcePrg, ExpectedErr);
}

TEST(Parse, DanglingDoWithStatement)
{
  std::string SourcePrg = "def foo do 3;";
  std::string ExpectedErr = "string stream:1:14: error: expected 'end' to end block";
  EXPECT_COMPILE_DEATH(SourcePrg, ExpectedErr);
}

TEST(Parse, EOFExpected)
{
  std::string SourcePrg = "def foo do 3; end;";
  std::string ExpectedErr = "string stream:1:18: error: expected end of file";
  EXPECT_COMPILE_DEATH(SourcePrg, ExpectedErr);
}

TEST(Parse, DanglingFunctionType)
{
  std::string SourcePrg = "def foo(A ~Function(Void ->";
  std::string ExpectedErr = "string stream:1:28: error: dangling function type specifier";
  EXPECT_COMPILE_DEATH(SourcePrg, ExpectedErr);
}

TEST(Parse, MalformedFunctionType)
{
  std::string SourcePrg = "def foo(A ~Function(Void ->)";
  std::string ExpectedErr = "string stream:1:28: error: dangling function type specifier";
  EXPECT_COMPILE_DEATH(SourcePrg, ExpectedErr);
}

TEST(Parse, FunctionTypeMissingOpenParen)
{
  std::string SourcePrg = "def foo(A ~Function) 3;";
  std::string ExpectedErr =
    "string stream:1:20: error: in function type of form 'Function\\(...\\)', '\\(' is missing";
  EXPECT_COMPILE_DEATH(SourcePrg, ExpectedErr);
}

TEST(Parse, FunctionTypeMissingCloseParen)
{
  std::string SourcePrg = "def foo(A ~Function(Void -> Void";
  std::string ExpectedErr =
    "string stream:1:33: error: in function type of form 'Function\\(...\\)', '\\)' is missing";
  EXPECT_COMPILE_DEATH(SourcePrg, ExpectedErr);
}

TEST(Parse, OnelinerRet)
{
  std::string SourcePrg = "def foo() do ret 3 end";
  std::string ExpectedIR = "";
  EXPECT_IR(SourcePrg, ExpectedIR);
}

TEST(Parse, MultilineDoBlock)
{
  std::string SourcePrg =
    "def foo() do\n"
    "  ret 3\n"
    "end";
  std::string ExpectedIR = "";
  EXPECT_IR(SourcePrg, ExpectedIR);
}

TEST(Parse, ExtraNewlines)
{
  std::string SourcePrg =
    "def foo() do\n\n\n\n"
    "  ret 3\n\n"
    "end\n\n";
  std::string ExpectedIR = "";
  EXPECT_IR(SourcePrg, ExpectedIR);
}

TEST(Parse, AssignmentArithOp)
{
  std::string SourcePrg =
    "def foo() do\n"
    "  Foo = 2 + 3\n"
    "  Bar = 5 - 19\n"
    "end";
  std::string ExpectedIR = "";
  EXPECT_IR(SourcePrg, ExpectedIR);
}

TEST(Parse, MixedSemicolonNewline)
{
  std::string SourcePrg =
    "def foo() do\n"
    "  Foo = 2; ret 3\n"
    "end";
  std::string ExpectedIR = "";
  EXPECT_IR(SourcePrg, ExpectedIR);
}

TEST(Parse, MissingStatementSeparator)
{
  std::string SourcePrg =
    "def foo() do\n"
    "  Foo = 2 ret 3\n"
    "end";
  std::string ExpectedErr =
    "string stream:2:11: error: expecting ';' or newline to terminate bind";
  EXPECT_COMPILE_DEATH(SourcePrg, ExpectedErr);
}
