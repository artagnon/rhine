#include "rhine/Util/TestUtil.hpp"
#include "gtest/gtest.h"

using namespace rhine;

TEST(Overshadow, Local) {
  auto SourcePrg = "def main do\n"
                   "  Handle = 0;\n"
                   "  Handle = 2;\n"
                   "end";
  auto ExpectedErr = "string stream:3:3: error: symbol Handle attempting to "
                     "overshadow previously bound symbol with same name";
  EXPECT_COMPILE_DEATH(SourcePrg, ExpectedErr);
}

TEST(Overshadow, Function) {
  auto SourcePrg = "def main do\n"
                   "  ret 3\n"
                   "end\n"
                   "def main do\n"
                   "  ret 2\n"
                   "end";
  auto ExpectedErr = "string stream:4:5: error: function main attempting to "
                     "overshadow previously bound symbol with same name";
  EXPECT_COMPILE_DEATH(SourcePrg, ExpectedErr);
}

TEST(Overshadow, Argument) {
  auto SourcePrg = "def main(main Int) do\n"
                   "  ret $ main + 2;\n"
                   "end";
  auto ExpectedErr = "string stream:1:10: error: argument main attempting to "
                     "overshadow previously bound symbol with same name";
  EXPECT_COMPILE_DEATH(SourcePrg, ExpectedErr);
}
