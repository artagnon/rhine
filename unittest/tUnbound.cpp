#include "rhine/Util/TestUtil.hpp"
#include "gtest/gtest.h"

using namespace rhine;

TEST(Unbound, Local) {
  auto SourcePrg = "def unboundVar do\n"
                   "  ret Var Int;\n"
                   "end";
  auto ExpectedErr = "string stream:2:7: error: unbound symbol Var";
  EXPECT_COMPILE_DEATH(SourcePrg, ExpectedErr);
}

TEST(Unbound, Function) {
  auto SourcePrg = "def main() do\n"
                   "  bar 4;\n"
                   "end";
  auto ExpectedErr = "string stream:2:3: error: unbound function bar";
  EXPECT_COMPILE_DEATH(SourcePrg, ExpectedErr);
}
