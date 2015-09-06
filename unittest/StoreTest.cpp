#include "rhine/Util/TestUtil.h"
#include "gtest/gtest.h"

using namespace rhine;

TEST(Store, DISABLED_Basic)
{
  std::string SourcePrg =
    "def main [] {\n"
    "  Handle = 0;\n"
    "  Handle = 2;\n"
    "}";
  std::string ExpectedLL = "store";
  EXPECT_LL(SourcePrg, ExpectedLL);
}

TEST(Store, DISABLED_CondAssign)
{
  std::string SourcePrg =
    "def main [] {\n"
    "  Handle = 0;\n"
    "  if (false) Handle = 2;\n"
    "  else Handle = 3;\n"
    "  ret Handle;\n"
    "}";
  std::string ExpectedOut = "3";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}
