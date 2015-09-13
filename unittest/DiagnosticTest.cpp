#include "rhine/Util/TestUtil.h"
#include "gtest/gtest.h"

using namespace rhine;

TEST(Diagnostic, UnboundVariable)
{
  std::string SourcePrg =
    "def unboundVar() do\n"
    "  ret Var ~Int;\n"
    "end";
  std::string ExpectedErr = "string stream:2:7: error: unbound symbol Var";
  EXPECT_COMPILE_DEATH(SourcePrg, ExpectedErr);
}

TEST(Diagnostic, UntypedArgument)
{
  std::string SourcePrg =
    "def untypedVar(Arg) do\n"
    "  ret Arg;\n"
    "end";
  std::string ExpectedErr =
    "string stream:1:16: error: untyped argument Arg";
  EXPECT_COMPILE_DEATH(SourcePrg, ExpectedErr);
}

TEST(Diagnostic, NotAFunction)
{
  std::string SourcePrg =
    "def main() do\n"
    "  foo = 2;\n"
    "  foo 4;\n"
    "end";
  std::string ExpectedErr =
    "string stream:3:3: error: foo was not typed as a function";
  EXPECT_COMPILE_DEATH(SourcePrg, ExpectedErr);
}

TEST(Diagnostic, FunctionNotFound)
{
  std::string SourcePrg =
    "def main() do\n"
    "  bar 4;\n"
    "end";
  std::string ExpectedErr =
    "string stream:2:3: error: unbound function bar";
  EXPECT_COMPILE_DEATH(SourcePrg, ExpectedErr);
}

TEST(Diagnostic, DISABLED_MissingOverloadedInstance)
{
  std::string SourcePrg =
    "def main() do\n"
    "  toString '4';\n"
    "end";
  std::string ExpectedErr =
    "error: no overloaded instance of toString takes String argument";
  EXPECT_COMPILE_DEATH(SourcePrg, ExpectedErr);
}
