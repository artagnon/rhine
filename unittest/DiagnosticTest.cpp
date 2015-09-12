#include "rhine/Util/TestUtil.h"
#include "gtest/gtest.h"

using namespace rhine;

TEST(Diagnostic, UnboundVariable)
{
  std::string SourcePrg = "def unboundVar [] ret Var ~Int;";
  std::string ExpectedErr = "string stream:1:23: error: unbound symbol Var";
  EXPECT_COMPILE_DEATH(SourcePrg, ExpectedErr);
}

TEST(Diagnostic, UntypedArgument)
{
  std::string SourcePrg = "def untypedVar [Arg] ret Arg;";
  std::string ExpectedErr =
    "string stream:1:17: error: untyped argument Arg";
  EXPECT_COMPILE_DEATH(SourcePrg, ExpectedErr);
}

TEST(Diagnostic, NotAFunction)
{
  std::string SourcePrg =
    "def main [] {\n"
    "  foo = 2;\n"
    "  foo 4;\n"
    "}";
  std::string ExpectedErr =
    "string stream:3:3: error: foo was not typed as a function";
  EXPECT_COMPILE_DEATH(SourcePrg, ExpectedErr);
}

TEST(Diagnostic, FunctionNotFound)
{
  std::string SourcePrg =
    "def main [] {\n"
    "  bar 4;\n"
    "}";
  std::string ExpectedErr =
    "string stream:2:3: error: unbound function bar";
  EXPECT_COMPILE_DEATH(SourcePrg, ExpectedErr);
}

TEST(Diagnostic, DISABLED_MissingOverloadedInstance)
{
  std::string SourcePrg =
    "def main [] {\n"
    "  toString '4';\n"
    "}";
  std::string ExpectedErr =
    "error: no overloaded instance of toString takes String argument";
  EXPECT_COMPILE_DEATH(SourcePrg, ExpectedErr);
}
