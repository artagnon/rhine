#include "rhine/Util/TestUtil.h"
#include "gtest/gtest.h"

using namespace rhine;

TEST(TyCoerce, ConstantIntToString) {
  auto SourcePrg =
    "def main do\n"
    "  print 62;\n"
    "end";
  auto ExpectedOut = "62";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}

TEST(TyCoerce, StringTyToString) {
  auto SourcePrg =
    "def boom(var ~String) do\n"
    "  print var;\n"
    "end\n"
    "def main do\n"
    "  boom '12';"
    "end";
  auto ExpectedOut = "12";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}

TEST(TyCoerce, IntTyToString) {
  auto SourcePrg =
    "def boom(var ~Int) do\n"
    "  print var;\n"
    "end\n"
    "def main do\n"
    "  boom 3;\n"
    "end";
  auto ExpectedOut = "3";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}

TEST(TyCoerce, InsideIf) {
  auto SourcePrg =
    "def main do\n"
    "  if false do print 2; else print 3; end\n"
    "end";
  auto ExpectedOut = "3";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}
