#include "rhine/TestUtil.h"
#include "gtest/gtest.h"

using namespace rhine;

TEST(IR, ConstantInt) {
  std::string SourcePrg = "def foo [] 3;";
  std::string ExpectedPP =
    "def foo [] ~Fn(() -> Int) {\n"
    "3 ~Int\n"
    "}";
  EXPECT_TRANSFORM_PP(SourcePrg, ExpectedPP);
}

TEST(IR, AddTwoInt)
{
  std::string SourcePrg = "def foo [] 3 + 2;";
  std::string ExpectedPP =
    "def foo [] ~Fn(() -> Int) {\n"
    "+ ~Fn(Int -> Int -> Int)\n"
    "3 ~Int\n"
    "2 ~Int\n"
    "}";
  EXPECT_TRANSFORM_PP(SourcePrg, ExpectedPP);
}

TEST(IR, ConstantString)
{
  std::string SourcePrg = "def foo [] 'moo!';";
  std::string ExpectedPP =
    "def foo [] ~Fn(() -> String) {\n"
    "'moo!' ~String\n"
    "}";
  EXPECT_TRANSFORM_PP(SourcePrg, ExpectedPP);
}

TEST(IR, TypePropagation)
{
  std::string SourcePrg = "def id [var ~Int] var;\n";
  std::string ExpectedPP =
    "def id [var ~Int] ~Fn(Int -> Int) {\n"
    "var ~Int\n"
    "}";
  EXPECT_TRANSFORM_PP(SourcePrg, ExpectedPP);
}

TEST(IR, MallocInst) {
  std::string SourcePrg = "def bsym [] sym = 3;";
  std::string ExpectedPP =
    "def bsym [] ~Fn(() -> ()) {\n"
    "sym = 3 ~Int\n"
    "}";
  EXPECT_TRANSFORM_PP(SourcePrg, ExpectedPP);
}

TEST(IR, BindPropagation) {
  std::string SourcePrg =
    "def bsym [] {\n"
    "  sym = 3;\n"
    "  sym;\n"
    "}";
  std::string ExpectedPP =
    "def bsym [] ~Fn(() -> Int) {\n"
    "sym = 3 ~Int\n"
    "sym ~Int\n"
    "}";
  EXPECT_TRANSFORM_PP(SourcePrg, ExpectedPP);
}

TEST(IR, Comment) {
  std::string SourcePrg =
    "def foo []\n"
    "  // Strip this out\n"
    "  3;";
  std::string ExpectedPP =
    "def foo [] ~Fn(() -> Int) {\n"
    "3 ~Int\n"
    "}";
  EXPECT_TRANSFORM_PP(SourcePrg, ExpectedPP);
}

TEST(IR, TwoArguments)
{
  std::string SourcePrg = "def foo [a ~Int b ~Int] a + b;";
  std::string ExpectedPP =
    "foo [a ~Int b ~Int] ~Fn(Int -> Int -> Int) {\n"
    "+ ~Fn(Int -> Int -> Int)\n"
    "a ~Int\n"
    "b ~Int\n"
    "}";
  EXPECT_TRANSFORM_PP(SourcePrg, ExpectedPP);
}

TEST(IR, TypePropagationCallInst)
{
  std::string SourcePrg = "def id [var ~String] println var;\n";
  std::string ExpectedPP =
    "def id [var ~String] ~Fn(String -> ()) {\n"
    "println ~Fn(String -> & -> ())*\n"
    "var ~String\n"
    "}";
  EXPECT_TRANSFORM_PP(SourcePrg, ExpectedPP);
}

TEST(IR, IfBasic)
{
  std::string SourcePrg =
    "def main [] { if (0) 2; else 3; }";
  std::string ExpectedPP =
    "def main [] ~Fn(() -> Int) {\n"
    "if (0 ~Int) {\n"
    "2 ~Int\n"
    "} else {\n"
    "3 ~Int\n"
    "}\n"
    "}";
  EXPECT_TRANSFORM_PP(SourcePrg, ExpectedPP);
}
