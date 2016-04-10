#include "rhine/Util/TestUtil.hpp"
#include "gtest/gtest.h"

using namespace rhine;

TEST(TypeCoerce, ConstantIntToString) {
  auto SourcePrg = "def main do\n"
                   "  print 62;\n"
                   "end";
  auto ExpectedOut = "62";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}

TEST(TypeCoerce, StringTyToString) {
  auto SourcePrg = "def boom(var String) do\n"
                   "  print var;\n"
                   "end\n"
                   "def main do\n"
                   "  boom '12';"
                   "end";
  auto ExpectedOut = "12";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}

TEST(TypeCoerce, IntTyToString) {
  auto SourcePrg = "def boom(var Int) do\n"
                   "  print var;\n"
                   "end\n"
                   "def main do\n"
                   "  boom 3;\n"
                   "end";
  auto ExpectedOut = "3";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}

TEST(TypeCoerce, Uncoercible) {
  auto SourcePrg = "def main do\n"
                   "  print print;\n"
                   "end";
  /// NOTE missing SourceLocation here
  EXPECT_COMPILE_DEATH(SourcePrg, "Unable to coerce argument from "
                                  "Fn\\(String -> & -> Void\\)\\* to String");
}

TEST(TypeCoerce, InsideIf) {
  auto SourcePrg = "def main do\n"
                   "  if false do print 2; else print 3; end\n"
                   "end";
  auto ExpectedOut = "3";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}
