#include "rhine/IR.h"
#include "rhine/Support.h"
#include "gtest/gtest.h"

using namespace rhine;

TEST(Diagnostic, BareDefun)
{
  std::string SourcePrg = "defun foo []";
  std::string ExpectedErr = "string stream:1:13: error: syntax error";
  EXPECT_DEATH(parseCodeGenString(SourcePrg), ExpectedErr);
}

TEST(Diagnostic, UnboundVariable)
{
  std::string SourcePrg = "defun foo [] var ~Int;";
  std::string ExpectedErr = "string stream:1:14: error: unbound symbol var";
  EXPECT_DEATH(parseCodeGenString(SourcePrg), ExpectedErr);
}

TEST(Diagnostic, UntypedVariable)
{
  std::string SourcePrg = "defun foo [var] var;";
  std::string ExpectedErr = "string stream:1:17: error: untyped symbol var";
  EXPECT_DEATH(parseCodeGenString(SourcePrg), ExpectedErr);
}

TEST(Diagnostic, NotAFunction)
{
  std::string SourcePrg =
    "defun main [] {\n"
    "  foo = 2;\n"
    "  foo 4;\n"
    "}\n";
  std::string ExpectedErr =
    "string stream:3:3: error: foo was not declared as a function";
  EXPECT_DEATH(parseCodeGenString(SourcePrg), ExpectedErr);
}

TEST(Diagnostic, FunctionNotFound)
{
  std::string SourcePrg =
    "defun main [] {\n"
    "  bar 4;\n"
    "}\n";
  std::string ExpectedErr =
    "string stream:2:3: error: unable to infer type of function bar";
  EXPECT_DEATH(parseCodeGenString(SourcePrg), ExpectedErr);
}

TEST(Diagnostic, BareDanglingBrace)
{
  std::string SourcePrg = "defun foo [] {";
  std::string ExpectedErr = "string stream:1:15: error: syntax error";
  EXPECT_DEATH(parseCodeGenString(SourcePrg), ExpectedErr);
}

TEST(Diagnostic, EmptyDefunBody)
{
  std::string SourcePrg = "defun foo [] {}";
  std::string ExpectedErr = "string stream:1:15: error: syntax error";
  EXPECT_DEATH(parseCodeGenString(SourcePrg), ExpectedErr);
}

TEST(Diagnostic, DanglingBraceWithStatement)
{
  std::string SourcePrg = "defun foo [] { 3;";
  std::string ExpectedErr = "string stream:1:18: error: syntax error";
  EXPECT_DEATH(parseCodeGenString(SourcePrg), ExpectedErr);
}
