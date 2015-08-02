#include "rhine/TestUtil.h"
#include "gtest/gtest.h"

using namespace rhine;

TEST(CodeGen, DefunStm)
{
  std::string SourcePrg = "def foo [] 3 + 2;";
  std::string ExpectedPP =
    "define i32 @foo() {\n"
    "entry:\n"
    "  ret i32 5\n"
    "}\n";
  EXPECT_PARSE_PP(SourcePrg, ExpectedPP);
}

TEST(CodeGen, DefunCompoundStm)
{
  std::string SourcePrg =
    "def foo []\n"
    "{\n"
    "  3 + 2;\n"
    "  4 + 5;\n"
    "}";
  std::string ExpectedPP =
    "define i32 @foo() {\n"
    "entry:\n"
    "  ret i32 9\n"
    "}\n";
  EXPECT_PARSE_PP(SourcePrg, ExpectedPP);
}

TEST(CodeGen, MultipleDefun)
{
  std::string SourcePrg =
    "def foo [] 2;\n"
    "def bar [] 3;\n";
  std::string ExpectedPP =
    "define i32 @foo() {\n"
    "entry:\n"
    "  ret i32 2\n"
    "}\n\n"
    "define i32 @bar() {\n"
    "entry:\n"
    "  ret i32 3\n"
    "}\n";
  EXPECT_PARSE_PP(SourcePrg, ExpectedPP);
}

TEST(CodeGen, TypeAnnotation)
{
  std::string SourcePrg =
    "def id [var ~Int] 0;\n";
  std::string ExpectedPP =
    "define i32 @id(i32) {\n"
    "entry:\n"
    "  ret i32 0\n"
    "}\n";
  EXPECT_PARSE_PP(SourcePrg, ExpectedPP);
}

TEST(CodeGen, FunctionArgBinding)
{
  std::string SourcePrg =
    "def id [var ~Int] var ~Int;\n";
  std::string ExpectedPP =
    "define i32 @id(i32) {\n"
    "entry:\n"
    "  ret i32 %0\n"
    "}\n";
  EXPECT_PARSE_PP(SourcePrg, ExpectedPP);
}

TEST(CodeGen, TypePropagation)
{
  std::string SourcePrg =
    "def id [var ~Int] var;\n";
  std::string ExpectedPP =
    "define i32 @id(i32) {\n"
    "entry:\n"
    "  ret i32 %0\n"
    "}\n";
  EXPECT_PARSE_PP(SourcePrg, ExpectedPP);
}

TEST(CodeGen, BindPropagation) {
  std::string SourcePrg =
    "def bsym [] {"
    "  Sym = 3;\n"
    "  Sym;\n"
    "}";
  std::string ExpectedPP =
    "define i32 @bsym() {\n"
    "entry:\n"
    "  %SymAlloca = alloca i32\n"
    "  store i32 3, i32* %SymAlloca\n"
    "  %SymLoad = load i32, i32* %SymAlloca\n"
    "  ret i32 %SymLoad\n"
    "}\n";
  EXPECT_PARSE_PP(SourcePrg, ExpectedPP);
}

TEST(CodeGen, FunctionCall)
{
  std::string SourcePrg =
    "def foom [] 2;\n"
    "def main [] foom;";
  std::string ExpectedPP =
    "ret i32 ()* @foom";
  EXPECT_PARSE_PP(SourcePrg, ExpectedPP);
}

TEST(CodeGen, VoidLowering)
{
  std::string SourcePrg =
    "def id [] Var = 3;\n";
  std::string ExpectedPP =
    "define void @id() {\n"
    "entry:\n"
    "  %VarAlloca = alloca i32\n"
    "  store i32 3, i32* %VarAlloca\n"
    "  ret void\n"
    "}\n";
  EXPECT_PARSE_PP(SourcePrg, ExpectedPP);
}

TEST(CodeGen, MultipleArguments)
{
  std::string SourcePrg =
    "def foo [a ~Int b ~Int] a + b;\n"
    "def main [] foo 3 2;";
  std::string ExpectedPP =
    "define i32 @foo(i32, i32) {\n"
    "entry:\n"
    "  %2 = add i32 %0, %1\n"
    "  ret i32 %2\n"
    "}\n\n"
    "define i32 @main() {\n"
    "entry:\n"
    "  %rhv1 = call i32 @foo(i32 3, i32 2)\n"
    "  ret i32 %rhv1\n"
    "}";
  EXPECT_PARSE_PP(SourcePrg, ExpectedPP);
}

TEST(CodeGen, ArgumentTypesChaining)
{
  std::string SourcePrg =
    "def boom [addfn ~Fn(Int -> Int -> Int)] addfn 2 4;";
  std::string ExpectedPP =
    "define i32 @boom(i32 (i32, i32)*)";
  EXPECT_PARSE_PP(SourcePrg, ExpectedPP);
}
