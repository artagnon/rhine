#include "rhine/Util/TestUtil.h"
#include "gtest/gtest.h"

using namespace rhine;

TEST(IR, ConstantInt) {
  auto SourcePrg =
    "def foo do\n"
    "  ret 3\n"
    "end";
  auto ExpectedIR =
    "def foo [] ~Fn(Void -> Int) {\n"
    "ret 3 ~Int\n"
    "}";
  EXPECT_IR(SourcePrg, ExpectedIR);
}

TEST(IR, AddTwoInt)
{
  auto SourcePrg =
    "def foo do\n"
    "  ret $ 3 + 2\n"
    "end";
  auto ExpectedIR =
    "ret + ~Fn(Int -> Int -> Int)\n"
    "3 ~Int\n"
    "2 ~Int\n";
  EXPECT_IR(SourcePrg, ExpectedIR);
}

TEST(IR, SubTwoInt)
{
  auto SourcePrg =
    "def foo ~Int do\n"
    "  ret $ 3 - 2\n"
    "end";
  auto ExpectedIR =
    "ret - ~Fn(Int -> Int -> Int)\n"
    "3 ~Int\n"
    "2 ~Int\n";
  EXPECT_IR(SourcePrg, ExpectedIR);
}

TEST(IR, ConstantString)
{
  auto SourcePrg =
    "def foo do\n"
    "  ret 'moo!'\n"
    "end";
  auto ExpectedIR =
    "def foo [] ~Fn(Void -> String) {\n"
    "ret 'moo!' ~String\n"
    "}";
  EXPECT_IR(SourcePrg, ExpectedIR);
}

TEST(IR, TypePropagation)
{
  auto SourcePrg =
    "def id(var ~Int) do\n"
    "  ret var\n"
    "end";
  auto ExpectedIR =
    "def id [var ~Int] ~Fn(Int -> Int) {\n"
    "ret var ~Int\n"
    "}";
  EXPECT_IR(SourcePrg, ExpectedIR);
}

TEST(IR, MallocInst) {
  auto SourcePrg =
    "def bsym do\n"
    "  sym = 3;"
    "end";
  auto ExpectedIR =
    "def bsym [] ~Fn(Void -> Void) {\n"
    "sym = 3 ~Int\n"
    "}";
  EXPECT_IR(SourcePrg, ExpectedIR);
}

TEST(IR, BindPropagation) {
  auto SourcePrg =
    "def bsym do\n"
    "  sym = 3\n"
    "  ret sym\n"
    "end";
  auto ExpectedIR =
    "def bsym [] ~Fn(Void -> Int) {\n"
    "sym = 3 ~Int\n"
    "ret sym ~Int\n"
    "}";
  EXPECT_IR(SourcePrg, ExpectedIR);
}

TEST(IR, Comment) {
  auto SourcePrg =
    "def foo do\n"
    "  // Strip this out\n"
    "  ret 3\n"
    "end";
  auto ExpectedIR =
    "def foo [] ~Fn(Void -> Int) {\n"
    "ret 3 ~Int\n"
    "}";
  EXPECT_IR(SourcePrg, ExpectedIR);
}

TEST(IR, TwoArguments)
{
  auto SourcePrg =
    "def foo(a ~Int b ~Int) do\n"
    "  ret $ a + b\n"
    "end";
  auto ExpectedIR =
    "ret + ~Fn(Int -> Int -> Int)\n"
    "a ~Int\n"
    "b ~Int\n";
  EXPECT_IR(SourcePrg, ExpectedIR);
}

TEST(IR, TypePropagationCallInst)
{
  auto SourcePrg =
    "def id(var ~String) do\n"
    "  println var\n"
    "end";
  auto ExpectedIR =
    "def id [var ~String] ~Fn(String -> Void) {\n"
    "println ~Fn(String -> & -> Void)*\n"
    "var ~String\n"
    "}";
  EXPECT_IR(SourcePrg, ExpectedIR);
}

TEST(IR, IfBasic)
{
  auto SourcePrg =
    "def main do\n"
    "  if false do X = 2; else Y = 3; end\n"
    "end";
  auto ExpectedIR =
    "def main [] ~Fn(Void -> Void) {\n"
    "if (0 ~Bool) {\n"
    "X = 2 ~Int\n"
    "} else {\n"
    "Y = 3 ~Int\n"
    "}\n"
    "}";
  EXPECT_IR(SourcePrg, ExpectedIR);
}
