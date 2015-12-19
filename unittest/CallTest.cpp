#include "rhine/Util/TestUtil.hpp"
#include "gtest/gtest.h"

using namespace rhine;

/// Stress testing call instructions with arity/types matching.

TEST(Call, DISABLED_MismatchedArity) {
  auto SourcePrg = "def main do\n"
                   "  println $ println 2\n"
                   "end";
  EXPECT_COMPILE_DEATH(SourcePrg, "foom");
}
