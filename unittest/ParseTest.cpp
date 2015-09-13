#include "rhine/Util/TestUtil.h"
#include "gtest/gtest.h"

using namespace rhine;

TEST(Parse, BareDefun)
{
  std::string SourcePrg = "def foo()";
  std::string ExpectedErr = "string stream:1:10: error: expecting a single statement";
  EXPECT_COMPILE_DEATH(SourcePrg, ExpectedErr);
}

TEST(Parse, BareDanglingBrace)
{
  std::string SourcePrg = "def foo() do";
  std::string ExpectedErr = "string stream:1:13: error: dangling block";
  EXPECT_COMPILE_DEATH(SourcePrg, ExpectedErr);
}

TEST(Parse, DanglingBraceWithStatement)
{
  std::string SourcePrg = "def foo() do 3;";
  std::string ExpectedErr = "string stream:1:16: error: dangling block";
  EXPECT_COMPILE_DEATH(SourcePrg, ExpectedErr);
}

TEST(Parse, EOFExpected)
{
  std::string SourcePrg = "def foo() do 3; end;";
  std::string ExpectedErr = "string stream:1:20: error: expected end of file";
  EXPECT_COMPILE_DEATH(SourcePrg, ExpectedErr);
}

TEST(Parse, DanglingFunctionType)
{
  std::string SourcePrg = "def foo(A ~Fn(Void ->";
  std::string ExpectedErr = "string stream:1:22: error: dangling function type specifier";
  EXPECT_COMPILE_DEATH(SourcePrg, ExpectedErr);
}

TEST(Parse, MalformedFunctionType)
{
  std::string SourcePrg = "def foo(A ~Fn(Void ->)";
  std::string ExpectedErr = "string stream:1:22: error: dangling function type specifier";
  EXPECT_COMPILE_DEATH(SourcePrg, ExpectedErr);
}

TEST(Parse, FunctionTypeMissingOpenParen)
{
  std::string SourcePrg = "def foo(A ~Fn) 3;";
  std::string ExpectedErr =
    "string stream:1:14: error: in function type of form 'Fn\\(...\\)', '\\(' is missing";
  EXPECT_COMPILE_DEATH(SourcePrg, ExpectedErr);
}

TEST(Parse, DISABLED_FunctionTypeMissingCloseParen)
{
  std::string SourcePrg = "def foo(A ~Fn(Void -> Void";
  std::string ExpectedErr =
    "string stream:1:15: error: in function type of form 'Fn\\(...\\)', '\\)' is missing";
  EXPECT_COMPILE_DEATH(SourcePrg, ExpectedErr);
}
