#include "rhine/Util/TestUtil.h"
#include "gtest/gtest.h"

using namespace rhine;

TEST(Store, Basic)
{
  std::string SourcePrg =
    "def main do\n"
    "  Handle = 0;\n"
    "  Handle = 2;\n"
    "  print Handle;\n"
    "end";
  std::string ExpectedLL =
    "  store i32 0, i32* %0\n"
    "  store i32 2, i32* %0";
  std::string ExpectedOut = "2";
  EXPECT_LL(SourcePrg, ExpectedLL);
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}

TEST(Store, CondAssign)
{
  std::string SourcePrg =
    "def main do\n"
    "  Handle = 0;\n"
    "  if false do\n"
    "    Handle = 2;\n"
    "  else\n"
    "    Handle = 3;\n"
    "  end\n"
    "  print Handle;\n"
    "end";
  std::string ExpectedOut = "3";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}
