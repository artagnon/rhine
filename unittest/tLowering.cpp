#include "rhine/Util/TestUtil.hpp"
#include "gtest/gtest.h"

using namespace rhine;

TEST(Lowering, LoweredValue_Cache) {
  auto SourcePrg = "def main do\n"
                   "  A = 3 + 2\n"
                   "  B = A + 1\n"
                   "end";
  EXPECT_LL<true>(SourcePrg, "%ALoad1 = load i32, i32* %0");
}
