#include "rhine/Util/TestUtil.hpp"
#include "gtest/gtest.h"

using namespace rhine;

TEST(Diagnostic, UnboundVariable)
{
  auto SourcePrg =
    "def unboundVar do\n"
    "  ret Var ~Int;\n"
    "end";
  auto ExpectedErr = "string stream:2:7: error: unbound symbol Var";
  EXPECT_COMPILE_DEATH(SourcePrg, ExpectedErr);
}

TEST(Diagnostic, UntypedArgument)
{
  auto SourcePrg =
    "def untypedVar(Arg) do\n"
    "  ret Arg;\n"
    "end";
  auto ExpectedErr =
    "string stream:1:16: error: untyped argument Arg";
  EXPECT_COMPILE_DEATH(SourcePrg, ExpectedErr);
}

TEST(Diagnostic, NotAFunction)
{
  auto SourcePrg =
    "def main() do\n"
    "  foo = 2;\n"
    "  foo 4;\n"
    "end";
  auto ExpectedErr =
    "string stream:3:3: error: foo was not typed as a function";
  EXPECT_COMPILE_DEATH(SourcePrg, ExpectedErr);
}

TEST(Diagnostic, FunctionNotFound)
{
  auto SourcePrg =
    "def main() do\n"
    "  bar 4;\n"
    "end";
  auto ExpectedErr =
    "string stream:2:3: error: unbound function bar";
  EXPECT_COMPILE_DEATH(SourcePrg, ExpectedErr);
}

TEST(Diagnostic, AlreadyBoundLocal)
{
  auto SourcePrg =
    "def main do\n"
    "  Handle = 0;\n"
    "  Handle = 2;\n"
    "end";
  auto ExpectedErr =
    "string stream:3:3: error: symbol Handle already bound";
  EXPECT_COMPILE_DEATH(SourcePrg, ExpectedErr);
}

TEST(Diagnostic, ConflictingFunctions)
{
  auto SourcePrg =
    "def main do\n"
    "  ret 3\n"
    "end\n"
    "def main do\n"
    "  ret 2\n"
    "end";
  auto ExpectedErr =
    "string stream:4:5: error: function main already exists";
  EXPECT_COMPILE_DEATH(SourcePrg, ExpectedErr);
}

TEST(Diagnostic, DISABLED_ArgumentNameEqualsFunctionName)
{
  auto SourcePrg =
    "def main(main ~Int) do\n"
    "  ret $ main + 2;\n"
    "end";
  auto ExpectedErr =
    "string stream:3:3: error: argument main conflicts with existing symbol name";
  EXPECT_COMPILE_DEATH(SourcePrg, ExpectedErr);
}

TEST(Diagnostic, DISABLED_MissingOverloadedInstance)
{
  auto SourcePrg =
    "def main() do\n"
    "  toString '4';\n"
    "end";
  auto ExpectedErr =
    "error: no overloaded instance of toString takes String argument";
  EXPECT_COMPILE_DEATH(SourcePrg, ExpectedErr);
}
