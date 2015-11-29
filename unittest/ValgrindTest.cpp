#include "rhine/Util/TestUtil.hpp"
#include "gtest/gtest.h"

using namespace rhine;

/// Stress reuse of values to make sure that the program is valgrind-clean.

TEST(Valgrind, Constant_MultiUse) {
  auto SourcePrg = "def main do\n"
                   "  print 2;\n"
                   "  print 2\n"
                   "end";
  EXPECT_OUTPUT(SourcePrg, "22");
}

TEST(Valgrind, Constant_MultiUse_MultiBB) {
  auto SourcePrg = "def main do\n"
                   "  if true do\n"
                   "    print 2\n"
                   "  else\n"
                   "    print 3\n"
                   "  end\n"
                   "  print 2\n"
                   "end";
  EXPECT_OUTPUT(SourcePrg, "22");
}

TEST(Valgrind, Constant_DefUse) {
  auto SourcePrg = "def main do\n"
                   "  X = 2\n"
                   "  print 2\n"
                   "end";
  EXPECT_OUTPUT(SourcePrg, "2");
}

TEST(Valgrind, Constant_Def_MultiUse) {
  auto SourcePrg = "def main do\n"
                   "  X = 2\n"
                   "  print 2;\n"
                   "  print 2\n"
                   "end";
  EXPECT_OUTPUT(SourcePrg, "22");
}

TEST(Valgrind, Constant_MultiBB) {
  auto SourcePrg = "def main do\n"
                   "  if true do\n"
                   "    X = 2\n"
                   "  end"
                   "  print 2\n"
                   "end";
  EXPECT_OUTPUT(SourcePrg, "2");
}

TEST(Valgrind, Value_SingleUse) {
  auto SourcePrg = "def main do\n"
                   "  X = 2\n"
                   "  print $ 9 + X\n"
                   "end";
  EXPECT_OUTPUT(SourcePrg, "11");
}

TEST(Valgrind, Value_MultiUse) {
  auto SourcePrg = "def main do\n"
                   "  X = 2\n"
                   "  Y = X + 3\n"
                   "  print $ 9 + X\n"
                   "end";
  EXPECT_OUTPUT(SourcePrg, "11");
}

TEST(Valgrind, Value_MultiBB) {
  auto SourcePrg = "def main do\n"
                   "  X = 2\n"
                   "  if true do\n"
                   "    print $ 9 + X\n"
                   "  end\n"
                   "end";
  EXPECT_OUTPUT(SourcePrg, "11");
}

TEST(Valgrind, Instruction_SingleUse) {
  auto SourcePrg = "def main do\n"
                   "  X = 2\n"
                   "  Y = X + 3\n"
                   "  print $ 9 + Y\n"
                   "end";
  EXPECT_OUTPUT(SourcePrg, "14");
}

TEST(Valgrind, Instruction_MultiUse) {
  auto SourcePrg = "def main do\n"
                   "  X = 2\n"
                   "  Y = X + 3\n"
                   "  Z = 18 + Y\n"
                   "  print $ 9 + Y\n"
                   "end";
  EXPECT_OUTPUT(SourcePrg, "14");
}

TEST(Valgrind, Instruction_MultiBB) {
  auto SourcePrg = "def main do\n"
                   "  X = 2\n"
                   "  Y = X + 3\n"
                   "  if true do\n"
                   "    print $ 9 + Y\n"
                   "  end\n"
                   "end";
  EXPECT_OUTPUT(SourcePrg, "14");
}

TEST(Valgrind, PhiAssignment) {
  auto SourcePrg = "def main do\n"
                   "  X =\n"
                   "    if false do 2\n"
                   "    else 3\n"
                   "    end\n"
                   "  print X\n"
                   "end";
  auto ExpectedOut = "3";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}
