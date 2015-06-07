#include "rhine/TestUtil.h"
#include "gtest/gtest.h"

using namespace rhine;

TEST(TyCoerce, Constant) {
  std::string SourcePrg =
    "def main [] {\n"
    "  println 62;\n"
    "}";
  std::string ExpectedOut = "62";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}

TEST(TyCoerce, Variable) {
  std::string SourcePrg =
    "def main [] {\n"
    "  println 62;\n"
    "}";
  std::string ExpectedOut = "62";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}
