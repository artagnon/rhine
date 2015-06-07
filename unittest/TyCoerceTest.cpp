#include "rhine/TestUtil.h"
#include "gtest/gtest.h"

using namespace rhine;

TEST(TyCoerce, ConstantIntToString) {
  std::string SourcePrg =
    "def main [] {\n"
    "  println 62;\n"
    "}";
  std::string ExpectedOut = "62";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}

TEST(TyCoerce, StringTyToString) {
  std::string SourcePrg =
    "def boom [var ~String] {\n"
    "  println var;\n"
    "}\n"
    "def main [] boom '12';";
  std::string ExpectedOut = "12";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}

TEST(TyCoerce, IntTyToString) {
  std::string SourcePrg =
    "def boom [var ~Int] {\n"
    "  println var;\n"
    "}\n"
    "def main [] boom 3;";
  std::string ExpectedOut = "3";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}
