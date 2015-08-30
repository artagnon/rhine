#include "rhine/TestUtil.h"
#include "gtest/gtest.h"

using namespace rhine;

TEST(TyCoerce, ConstantIntToString) {
  std::string SourcePrg =
    "def main [] {\n"
    "  print 62;\n"
    "}";
  std::string ExpectedOut = "62";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}

TEST(TyCoerce, StringTyToString) {
  std::string SourcePrg =
    "def boom [var ~String] {\n"
    "  print var;\n"
    "}\n"
    "def main [] boom '12';";
  std::string ExpectedOut = "12";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}

TEST(TyCoerce, IntTyToString) {
  std::string SourcePrg =
    "def boom [var ~Int] {\n"
    "  print var;\n"
    "}\n"
    "def main [] boom 3;";
  std::string ExpectedOut = "3";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}

TEST(TyCoerce, InsideIf)
{
  std::string SourcePrg =
    "def main [] { if (false) print 2; else print 3; }";
  std::string ExpectedOut = "3";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}
