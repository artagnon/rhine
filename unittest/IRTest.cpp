#include "rhine/Util/TestUtil.h"
#include "gtest/gtest.h"

using namespace rhine;

TEST(IR, ConstantInt) {
  std::string SourcePrg =
    "def foo() do\n"
    "  ret 3;\n"
    "end";
  std::string ExpectedIR =
    "def foo [] ~Fn(Void -> Int) {\n"
    "ret 3 ~Int\n"
    "}";
  EXPECT_IR(SourcePrg, ExpectedIR);
}

TEST(IR, AddTwoInt)
{
  std::string SourcePrg =
    "def foo() do\n"
    "  3 + 2;\n"
    "end";
  std::string ExpectedIR =
    "def foo [] ~Fn(Void -> Int) {\n"
    "+ ~Fn(Int -> Int -> Int)\n"
    "3 ~Int\n"
    "2 ~Int\n"
    "}";
  EXPECT_IR(SourcePrg, ExpectedIR);
}

TEST(IR, SubTwoInt)
{
  std::string SourcePrg =
    "def foo() do\n"
    "  3 - 2;\n"
    "end";
  std::string ExpectedIR =
    "def foo [] ~Fn(Void -> Int) {\n"
    "- ~Fn(Int -> Int -> Int)\n"
    "3 ~Int\n"
    "2 ~Int\n"
    "}";
  EXPECT_IR(SourcePrg, ExpectedIR);
}

TEST(IR, DISABLED_FcnTypeSpecifier)
{
  std::string SourcePrg =
    "def foo() ~Fn(Void -> Int) do\n"
    "  ret $ 3 - 2;\n"
    "end";
  std::string ExpectedIR =
    "def foo [] ~Fn(Void -> Int) {\n"
    "- ~Fn(Int -> Int -> Int)\n"
    "3 ~Int\n"
    "2 ~Int\n"
    "}";
  EXPECT_IR(SourcePrg, ExpectedIR);
}

TEST(IR, ConstantString)
{
  std::string SourcePrg =
    "def foo() do\n"
    "  ret 'moo!';\n"
    "end";
  std::string ExpectedIR =
    "def foo [] ~Fn(Void -> String) {\n"
    "ret 'moo!' ~String\n"
    "}";
  EXPECT_IR(SourcePrg, ExpectedIR);
}

TEST(IR, TypePropagation)
{
  std::string SourcePrg =
    "def id(var ~Int) do\n"
    "  ret var;\n"
    "end";
  std::string ExpectedIR =
    "def id [var ~Int] ~Fn(Int -> Int) {\n"
    "ret var ~Int\n"
    "}";
  EXPECT_IR(SourcePrg, ExpectedIR);
}

TEST(IR, MallocInst) {
  std::string SourcePrg =
    "def bsym() do\n"
    "  sym = 3;"
    "end";
  std::string ExpectedIR =
    "def bsym [] ~Fn(Void -> Void) {\n"
    "sym = 3 ~Int\n"
    "}";
  EXPECT_IR(SourcePrg, ExpectedIR);
}

TEST(IR, BindPropagation) {
  std::string SourcePrg =
    "def bsym() do\n"
    "  sym = 3;\n"
    "  ret sym;\n"
    "end";
  std::string ExpectedIR =
    "def bsym [] ~Fn(Void -> Int) {\n"
    "sym = 3 ~Int\n"
    "ret sym ~Int\n"
    "}";
  EXPECT_IR(SourcePrg, ExpectedIR);
}

TEST(IR, Comment) {
  std::string SourcePrg =
    "def foo() do\n"
    "  // Strip this out\n"
    "  ret 3;\n"
    "end";
  std::string ExpectedIR =
    "def foo [] ~Fn(Void -> Int) {\n"
    "ret 3 ~Int\n"
    "}";
  EXPECT_IR(SourcePrg, ExpectedIR);
}

TEST(IR, TwoArguments)
{
  std::string SourcePrg =
    "def foo(a ~Int b ~Int) do"
    "  ret $ a + b;"
    "end";
  std::string ExpectedIR =
    "foo [a ~Int b ~Int] ~Fn(Int -> Int -> Int) {\n"
    "ret + ~Fn(Int -> Int -> Int)\n"
    "a ~Int\n"
    "b ~Int\n"
    "}";
  EXPECT_IR(SourcePrg, ExpectedIR);
}

TEST(IR, TypePropagationCallInst)
{
  std::string SourcePrg =
    "def id(var ~String) do\n"
    "  println var;\n"
    "end";
  std::string ExpectedIR =
    "def id [var ~String] ~Fn(String -> Void) {\n"
    "println ~Fn(String -> & -> Void)*\n"
    "var ~String\n"
    "}";
  EXPECT_IR(SourcePrg, ExpectedIR);
}

TEST(IR, DISABLED_IfBasic)
{
  std::string SourcePrg =
    "def main() do\n"
    "  if (0) 2; else 3;\n"
    "end";
  std::string ExpectedIR =
    "def main [] ~Fn(Void -> Void) {\n"
    "if (0 ~Int) {\n"
    "2 ~Int\n"
    "} else {\n"
    "3 ~Int\n"
    "}\n"
    "}";
  EXPECT_IR(SourcePrg, ExpectedIR);
}
