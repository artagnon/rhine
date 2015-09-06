#include "rhine/Util/TestUtil.h"
#include "gtest/gtest.h"

using namespace rhine;

TEST(CodeGen, DefunStm)
{
  std::string SourcePrg = "def foo [] ret $ 3 + 2;";
  std::string ExpectedLL =
    "define i32 @foo() gc \"rhgc\" {\n"
    "entry:\n"
    "  ret i32 5\n"
    "}\n";
  EXPECT_LL(SourcePrg, ExpectedLL);
}

TEST(CodeGen, DefunCompoundStm)
{
  std::string SourcePrg =
    "def foo []\n"
    "{\n"
    "  3 + 2;\n"
    "  ret $ 4 + 5;\n"
    "}";
  std::string ExpectedLL =
    "define i32 @foo() gc \"rhgc\" {\n"
    "entry:\n"
    "  ret i32 9\n"
    "}\n";
  EXPECT_LL(SourcePrg, ExpectedLL);
}

TEST(CodeGen, MultipleDefun)
{
  std::string SourcePrg =
    "def foo [] ret 2;\n"
    "def bar [] ret 3;\n";
  std::string ExpectedLL =
    "define i32 @foo() gc \"rhgc\" {\n"
    "entry:\n"
    "  ret i32 2\n"
    "}\n\n"
    "define i32 @bar() gc \"rhgc\" {\n"
    "entry:\n"
    "  ret i32 3\n"
    "}\n";
  EXPECT_LL(SourcePrg, ExpectedLL);
}

TEST(CodeGen, TypeAnnotation)
{
  std::string SourcePrg =
    "def id [var ~Int] ret 0;\n";
  std::string ExpectedLL =
    "define i32 @id(i32) gc \"rhgc\" {\n"
    "entry:\n"
    "  ret i32 0\n"
    "}\n";
  EXPECT_LL(SourcePrg, ExpectedLL);
}

TEST(CodeGen, FunctionArgBinding)
{
  std::string SourcePrg =
    "def id [var ~Int] ret var ~Int;\n";
  std::string ExpectedLL =
    "define i32 @id(i32) gc \"rhgc\" {\n"
    "entry:\n"
    "  ret i32 %0\n"
    "}\n";
  EXPECT_LL(SourcePrg, ExpectedLL);
}

TEST(CodeGen, TypePropagation)
{
  std::string SourcePrg =
    "def id [var ~Int] ret var;\n";
  std::string ExpectedLL =
    "define i32 @id(i32) gc \"rhgc\" {\n"
    "entry:\n"
    "  ret i32 %0\n"
    "}\n";
  EXPECT_LL(SourcePrg, ExpectedLL);
}

TEST(CodeGen, BindPropagation) {
  std::string SourcePrg =
    "def bsym [] {"
    "  Sym = 3;\n"
    "  ret Sym;\n"
    "}";
  std::string ExpectedLL =
    "define i32 @bsym() gc \"rhgc\" {\n"
    "entry:\n"
    "  %Alloc = call i8* @std_String_malloc__Int(i64 4)\n"
    "  %0 = bitcast i8* %Alloc to i32*\n"
    "  store i32 3, i32* %0\n"
    "  %SymLoad = load i32, i32* %0\n"
    "  ret i32 %SymLoad\n"
    "}\n";
  EXPECT_LL(SourcePrg, ExpectedLL);
}

TEST(CodeGen, FunctionCall)
{
  std::string SourcePrg =
    "def foom [] ret 2;\n"
    "def main [] ret $ foom ();";
  std::string ExpectedLL =
    "ret i32 %foom";
  EXPECT_LL(SourcePrg, ExpectedLL);
}

TEST(CodeGen, VoidLowering)
{
  std::string SourcePrg =
    "def id [] { Var = 3; ret (); }\n";
  std::string ExpectedLL =
    "define void @id() gc \"rhgc\" {\n"
    "entry:\n"
    "  %Alloc = call i8* @std_String_malloc__Int(i64 4)\n"
    "  %0 = bitcast i8* %Alloc to i32*\n"
    "  store i32 3, i32* %0\n"
    "  ret void\n"
    "}\n";
  EXPECT_LL(SourcePrg, ExpectedLL);
}

TEST(CodeGen, MultipleArguments)
{
  std::string SourcePrg =
    "def foo [a ~Int b ~Int] ret $ a + b;\n"
    "def main [] ret $ foo 3 2;";
  std::string ExpectedLL =
    "define i32 @foo(i32, i32) gc \"rhgc\" {\n"
    "entry:\n"
    "  %2 = add i32 %0, %1\n"
    "  ret i32 %2\n"
    "}\n\n"
    "define i32 @main() gc \"rhgc\" {\n"
    "entry:\n"
    "  %foo = call i32 @foo(i32 3, i32 2)\n"
    "  ret i32 %foo\n"
    "}";
  EXPECT_LL(SourcePrg, ExpectedLL);
}

TEST(CodeGen, ArgumentTypesChaining)
{
  std::string SourcePrg =
    "def boom [addfn ~Fn(Int -> Int -> Int)] ret $ addfn 2 4;";
  std::string ExpectedLL =
    "define i32 @boom(i32 (i32, i32)*)";
  EXPECT_LL(SourcePrg, ExpectedLL);
}
