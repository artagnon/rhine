#include "rhine/Util/TestUtil.h"
#include "gtest/gtest.h"

using namespace rhine;

TEST(TyCoerce, ConstantIntToString) {
  std::string SourcePrg =
    "def main() do\n"
    "  print 62;\n"
    "end";
  std::string ExpectedOut = "62";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}

TEST(TyCoerce, StringTyToString) {
  std::string SourcePrg =
    "def boom(var ~String) do\n"
    "  print var;\n"
    "end\n"
    "def main() do\n"
    "  boom '12';"
    "end";
  std::string ExpectedOut = "12";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}

TEST(TyCoerce, IntTyToString) {
  std::string SourcePrg =
    "def boom(var ~Int) do\n"
    "  print var;\n"
    "end\n"
    "def main() do\n"
    "  boom 3;\n"
    "end";
  std::string ExpectedOut = "3";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}

TEST(TyCoerce, DISABLED_InsideIf) {
  std::string SourcePrg =
    "def main() do\n"
    "  if (false) print 2; else print 3;\n"
    "end";
  std::string ExpectedOut = "3";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}
