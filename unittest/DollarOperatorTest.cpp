#include "rhine/Util/TestUtil.h"
#include "gtest/gtest.h"

using namespace rhine;

TEST(DollarOperator, Basic) {
  std::string SourcePrg =
    "def foo(x ~Int) do\n"
    "  ret '2';\n"
    "end\n"
    "def main() do\n"
    "  print $ foo 3;"
    "end";
  std::string ExpectedOut = "2";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}
