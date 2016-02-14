#include "rhine/Util/TestUtil.hpp"
#include "gtest/gtest.h"

using namespace rhine;

TEST(Tensor, Empty) {
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

TEST(Tensor, Indexing_SingleElement) {
  auto SourcePrg = "def main do\n"
                   "A = {42}\n"
                   "print A[0]\n"
                   "end";
  EXPECT_OUTPUT(SourcePrg, "42");
}

TEST(Tensor, Indexing) {
  auto SourcePrg = "def main do\n"
                   "A = {42, 56, 91, 23}\n"
                   "print A[2]\n"
                   "end";
  EXPECT_OUTPUT(SourcePrg, "91");
}

TEST(Tensor, DISABLED_Multidimensional) {
  auto SourcePrg = "def main do\n"
                   "A = {{42, 56}, {91, 23}}\n"
                   "print A[0][1]\n"
                   "end";
  EXPECT_OUTPUT(SourcePrg, "56");
}
