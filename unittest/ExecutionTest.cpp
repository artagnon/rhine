#include "rhine/Util/TestUtil.h"
#include "gtest/gtest.h"

using namespace rhine;

TEST(Execution, DollarOperator) {
  std::string SourcePrg =
    "def foo [x ~Int] ret '2';\n"
    "def main [] print $ foo 3;";
  std::string ExpectedOut = "2";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}
