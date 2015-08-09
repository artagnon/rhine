#include "rhine/Toplevel/ParseFacade.h"
#include "gtest/gtest.h"

using namespace rhine;

void EXPECT_COMPILE_DEATH(std::string &SourcePrg, std::string &ExpectedErr)
{
  auto Pf = ParseFacade(SourcePrg);
  EXPECT_DEATH(
      Pf.parseAction(ParseSource::STRING, PostParseAction::LL), ExpectedErr);
}

TEST(Diagnostic, BareDefun)
{
  std::string SourcePrg = "def foo []";
  std::string ExpectedErr = "string stream:1:11: error: syntax error";
  EXPECT_COMPILE_DEATH(SourcePrg, ExpectedErr);
}

TEST(Diagnostic, DISABLED_UnboundVariable)
{
  std::string SourcePrg = "def unboundVar [] var ~Int;";
  std::string ExpectedErr = "string stream:1:19: error: unbound symbol var";
  EXPECT_COMPILE_DEATH(SourcePrg, ExpectedErr);
}

TEST(Diagnostic, UntypedVariable)
{
  std::string SourcePrg = "def untypedVar [var] var;";
  std::string ExpectedErr =
    "string stream:1:17: error: untyped argument var";
  EXPECT_COMPILE_DEATH(SourcePrg, ExpectedErr);
}

TEST(Diagnostic, NotAFunction)
{
  std::string SourcePrg =
    "def main [] {\n"
    "  foo = 2;\n"
    "  foo 4;\n"
    "}\n";
  std::string ExpectedErr =
    "string stream:3:3: error: foo was not typed as a function";
  EXPECT_COMPILE_DEATH(SourcePrg, ExpectedErr);
}

TEST(Diagnostic, FunctionNotFound)
{
  std::string SourcePrg =
    "def main [] {\n"
    "  bar 4;\n"
    "}\n";
  std::string ExpectedErr =
    "string stream:2:3: error: untyped function bar";
  EXPECT_COMPILE_DEATH(SourcePrg, ExpectedErr);
}

TEST(Diagnostic, BareDanglingBrace)
{
  std::string SourcePrg = "def foo [] {";
  std::string ExpectedErr = "string stream:1:13: error: syntax error";
  EXPECT_COMPILE_DEATH(SourcePrg, ExpectedErr);
}

TEST(Diagnostic, DanglingBraceWithStatement)
{
  std::string SourcePrg = "def foo [] { 3;";
  std::string ExpectedErr = "string stream:1:16: error: syntax error";
  EXPECT_COMPILE_DEATH(SourcePrg, ExpectedErr);
}
