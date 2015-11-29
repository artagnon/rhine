#include "rhine/Util/TestUtil.hpp"
#include "gtest/gtest.h"

using namespace rhine;

TEST(Store, Basic)
{
  auto SourcePrg =
    "def main do\n"
    "  Handle = 0;\n"
    "  Handle := 2;\n"
    "  print Handle;\n"
    "end";
  auto ExpectedLL =
    "  store i32 0, i32* %0\n"
    "  store i32 2, i32* %0";
  auto ExpectedOut = "2";
  EXPECT_LL(SourcePrg, ExpectedLL);
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}

TEST(Store, CondAssign)
{
  auto SourcePrg =
    "def main do\n"
    "  Handle = 0;\n"
    "  if false do\n"
    "    Handle := 2;\n"
    "  else\n"
    "    Handle := 3;\n"
    "  end\n"
    "  print Handle;\n"
    "end";
  auto ExpectedOut = "3";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}
