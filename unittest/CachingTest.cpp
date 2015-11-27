#include "rhine/Util/TestUtil.h"
#include "gtest/gtest.h"

using namespace rhine;

/// Stress the lowering to make sure that no value is re-computed unnecessarily.

TEST(Caching, Value_SingleUse) {
  auto SourcePrg = "def main do\n"
                   "  X = 2\n"
                   "  print $ 9 + X\n"
                   "end";
  EXPECT_LL(SourcePrg, "11");
}

TEST(Caching, Value_MultiUse) {
  auto SourcePrg = "def main do\n"
                   "  X = 2\n"
                   "  Y = X + 3\n"
                   "  print $ 9 + X\n"
                   "end";
 EXPECT_LL(SourcePrg, "11");
}

TEST(Caching, Instruction_SingleUse) {
  auto SourcePrg = "def main do\n"
                   "  X = 2\n"
                   "  Y = X + 3\n"
                   "  print $ 9 + Y\n"
                   "end";
  EXPECT_LL(SourcePrg, "14");
}

TEST(Caching, Instruction_MultiUse) {
  auto SourcePrg = "def main do\n"
                   "  X = 2\n"
                   "  Y = X + 3\n"
                   "  Z = 18 + Y\n"
                   "  print $ 9 + Y\n"
                   "end";
  EXPECT_LL(SourcePrg, "14");
}
