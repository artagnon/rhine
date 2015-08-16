#include "rhine/TestUtil.h"
#include "gtest/gtest.h"

using namespace rhine;

TEST(IR, ConstantInt) {
  std::string SourcePrg = "def foo [] ret 3;";
  std::string ExpectedPP =
    "def foo [] ~Fn(Void -> Int) {\n"
    "ret 3 ~Int\n"
    "}";
  EXPECT_TRANSFORM_PP(SourcePrg, ExpectedPP);
}

TEST(IR, AddTwoInt)
{
  std::string SourcePrg = "def foo [] 3 + 2;";
  std::string ExpectedPP =
    "def foo [] ~Fn(Void -> Int) {\n"
    "+ ~Fn(Int -> Int -> Int)\n"
    "3 ~Int\n"
    "2 ~Int\n"
    "}";
  EXPECT_TRANSFORM_PP(SourcePrg, ExpectedPP);
}

TEST(IR, ConstantString)
{
  std::string SourcePrg = "def foo [] ret 'moo!';";
  std::string ExpectedPP =
    "def foo [] ~Fn(Void -> String) {\n"
    "ret 'moo!' ~String\n"
    "}";
  EXPECT_TRANSFORM_PP(SourcePrg, ExpectedPP);
}

TEST(IR, TypePropagation)
{
  std::string SourcePrg = "def id [var ~Int] ret var;\n";
  std::string ExpectedPP =
    "def id [var ~Int] ~Fn(Int -> Int) {\n"
    "ret var ~Int\n"
    "}";
  EXPECT_TRANSFORM_PP(SourcePrg, ExpectedPP);
}

TEST(IR, MallocInst) {
  std::string SourcePrg = "def bsym [] sym = 3;";
  std::string ExpectedPP =
    "def bsym [] ~Fn(Void -> Void) {\n"
    "sym = 3 ~Int\n"
    "}";
  EXPECT_TRANSFORM_PP(SourcePrg, ExpectedPP);
}

TEST(IR, BindPropagation) {
  std::string SourcePrg =
    "def bsym [] {\n"
    "  sym = 3;\n"
    "  ret sym;\n"
    "}";
  std::string ExpectedPP =
    "def bsym [] ~Fn(Void -> Int) {\n"
    "sym = 3 ~Int\n"
    "ret sym ~Int\n"
    "}";
  EXPECT_TRANSFORM_PP(SourcePrg, ExpectedPP);
}

TEST(IR, Comment) {
  std::string SourcePrg =
    "def foo []\n"
    "  // Strip this out\n"
    "  ret 3;";
  std::string ExpectedPP =
    "def foo [] ~Fn(Void -> Int) {\n"
    "ret 3 ~Int\n"
    "}";
  EXPECT_TRANSFORM_PP(SourcePrg, ExpectedPP);
}

TEST(IR, TwoArguments)
{
  std::string SourcePrg = "def foo [a ~Int b ~Int] ret $ a + b;";
  std::string ExpectedPP =
    "foo [a ~Int b ~Int] ~Fn(Int -> Int -> Int) {\n"
    "ret + ~Fn(Int -> Int -> Int)\n"
    "a ~Int\n"
    "b ~Int\n"
    "}";
  EXPECT_TRANSFORM_PP(SourcePrg, ExpectedPP);
}

TEST(IR, TypePropagationCallInst)
{
  std::string SourcePrg = "def id [var ~String] println var;\n";
  std::string ExpectedPP =
    "def id [var ~String] ~Fn(String -> Void) {\n"
    "println ~Fn(String -> & -> Void)*\n"
    "var ~String\n"
    "}";
  EXPECT_TRANSFORM_PP(SourcePrg, ExpectedPP);
}

TEST(IR, IfBasic)
{
  std::string SourcePrg =
    "def main [] { if (0) 2; else 3; }";
  std::string ExpectedPP =
    "def main [] ~Fn(Void -> Int) {\n"
    "if (0 ~Int) {\n"
    "2 ~Int\n"
    "} else {\n"
    "3 ~Int\n"
    "}\n"
    "}";
  EXPECT_TRANSFORM_PP(SourcePrg, ExpectedPP);
}
