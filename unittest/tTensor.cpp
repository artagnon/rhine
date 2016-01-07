#include "rhine/Util/TestUtil.hpp"
#include "gtest/gtest.h"

using namespace rhine;

TEST(Tensor, DISABLED_Empty) {
  auto SourcePrg = "def foo do\n"
                   "A = {}\n"
                   "end";
  EXPECT_SUCCESSFUL_PARSE(SourcePrg);
}

TEST(Tensor, SingleElement) {
  auto SourcePrg = "def foo do\n"
                   "A = {0}\n"
                   "end";
  EXPECT_SUCCESSFUL_PARSE(SourcePrg);
}
