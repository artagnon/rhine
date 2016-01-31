#include "rhine/Util/TestUtil.hpp"
#include "gtest/gtest.h"

using namespace rhine;

TEST(Diagnostic, UntypedArgument) {
  auto SourcePrg = "def untypedVar(Arg) do\n"
                   "  ret Arg;\n"
                   "end";
  auto ExpectedErr = "string stream:1:16: error: untyped argument Arg";
  EXPECT_COMPILE_DEATH(SourcePrg, ExpectedErr);
}

TEST(Diagnostic, NotAFunction) {
  auto SourcePrg = "def main() do\n"
                   "  foo = 2;\n"
                   "  foo 4;\n"
                   "end";
  auto ExpectedErr =
      "string stream:3:3: error: foo was expected to be a pointer to a function"
      " but was instead found to be of type Int";
  EXPECT_COMPILE_DEATH(SourcePrg, ExpectedErr);
}
