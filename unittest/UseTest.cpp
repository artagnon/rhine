#include "rhine/Util/TestUtil.h"
#include "gtest/gtest.h"

using namespace rhine;

/// Stress the destructors to clean up multiple uses correctly.

TEST(Use, Value_SingleUse) {
  auto SourcePrg = "def main do\n"
                   "  X = 2\n"
                   "  print $ 9 + X\n"
                   "end";
  EXPECT_OUTPUT(SourcePrg, "11");
}

TEST(Use, Value_MultiUse) {
  auto SourcePrg = "def main do\n"
                   "  X = 2\n"
                   "  Y = X + 3\n"
                   "  print $ 9 + X\n"
                   "end";
 EXPECT_OUTPUT(SourcePrg, "11");
}

TEST(Use, Instruction_SingleUse) {
  auto SourcePrg = "def main do\n"
                   "  X = 2\n"
                   "  Y = X + 3\n"
                   "  print $ 9 + Y\n"
                   "end";
  EXPECT_OUTPUT(SourcePrg, "14");
}

TEST(Use, Instruction_MultiUse) {
  auto SourcePrg = "def main do\n"
                   "  X = 2\n"
                   "  Y = X + 3\n"
                   "  Z = 18 + Y\n"
                   "  print $ 9 + Y\n"
                   "end";
  EXPECT_OUTPUT(SourcePrg, "14");
}
