#include "rhine/Util/TestUtil.hpp"
#include "gtest/gtest.h"

using namespace rhine;

TEST(Overload, DISABLED_Missing) {
  auto SourcePrg = "def main() do\n"
                   "  toString '4';\n"
                   "end";
  auto ExpectedErr =
      "error: no overloaded instance of toString takes String argument";
  EXPECT_COMPILE_DEATH(SourcePrg, ExpectedErr);
}
