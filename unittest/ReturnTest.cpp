#include "rhine/Util/TestUtil.hpp"
#include "gtest/gtest.h"

using namespace rhine;

TEST(Return, Assignment) {
  auto SourcePrg = "def main() do\n"
                   "  ret $ X = 3;\n"
                   "end";
  auto Err = "string stream:2:9: error: cannot return expression of Void type";
  EXPECT_COMPILE_DEATH(SourcePrg, Err);
}

TEST(Return, VoidFunction) {
  auto SourcePrg = "def foo() do ret () end\n"
                   "def main() do\n"
                   "  ret $ foo ();\n"
                   "end";
  auto Err = "string stream:3:9: error: cannot return expression of Void type";
  EXPECT_COMPILE_DEATH(SourcePrg, Err);
}

TEST(Return, DISABLED_Dollar_Return) {
  auto SourcePrg = "def main() do\n"
                   "  ret $ ret 3;\n"
                   "end";
  auto Err = "string stream:2:7";
  EXPECT_COMPILE_DEATH(SourcePrg, Err);
}

TEST(Return, EarlyExit) {
  auto SourcePrg = "def foo() do ret () end\n"
                   "def main() do\n"
                   "  ret $ foo ()\n"
                   "  X = 2\n"
                   "end";
  auto Err = "string stream:3:3: error: unexpected block terminator";
  EXPECT_COMPILE_DEATH(SourcePrg, Err);
}
