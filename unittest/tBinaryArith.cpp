#include "rhine/Util/TestUtil.hpp"
#include "gtest/gtest.h"

using namespace rhine;

TEST(BinaryArith, Add)
{
  auto SourcePrg =
    "def foo do\n"
    "  ret $ 3 + 2\n"
    "end";
  auto ExpectedBinaryArith =
    "ret + Int\n"
    "3 Int\n"
    "2 Int\n";
  EXPECT_IR(SourcePrg, ExpectedBinaryArith);
}

TEST(BinaryArith, Sub)
{
  auto SourcePrg =
    "def foo Int do\n"
    "  ret $ 3 - 2\n"
    "end";
  auto ExpectedBinaryArith =
    "ret - Int\n"
    "3 Int\n"
    "2 Int\n";
  EXPECT_IR(SourcePrg, ExpectedBinaryArith);
}

TEST(BinaryArith, Mul)
{
  auto SourcePrg =
    "def foo Int do\n"
    "  ret $ 3 * 2\n"
    "end";
  auto ExpectedBinaryArith =
    "ret * Int\n"
    "3 Int\n"
    "2 Int\n";
  EXPECT_IR(SourcePrg, ExpectedBinaryArith);
}

TEST(BinaryArith, Div)
{
  auto SourcePrg =
    "def foo Int do\n"
    "  ret $ 3 / 2\n"
    "end";
  auto ExpectedBinaryArith =
    "ret / Int\n"
    "3 Int\n"
    "2 Int\n";
  EXPECT_IR(SourcePrg, ExpectedBinaryArith);
}
