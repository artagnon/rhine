#include "rhine/Util/TestUtil.hpp"
#include "gtest/gtest.h"

using namespace rhine;

/// Stress testing call instructions with arity/types matching.

TEST(Call, MismatchedArity) {
  auto SourcePrg = "def main do\n"
                   "  println 2 3\n"
                   "end";
  EXPECT_COMPILE_DEATH(
      SourcePrg,
      "string stream:2:3: error: Call expected 1 number of arguments,"
      " but has been supplied 2 arguments");
}

TEST(Call, UncoercibleType) {
  auto SourcePrg = "def main do\n"
                   "  println $ println 3\n"
                   "end";
  EXPECT_COMPILE_DEATH(SourcePrg, "string stream:2:13: error: Unable to coerce "
                                  "argument from Void to String");
}
