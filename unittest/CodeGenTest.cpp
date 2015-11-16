#include "rhine/Util/TestUtil.h"
#include "gtest/gtest.h"

using namespace rhine;

TEST(CodeGen, TypeAnnotation)
{
  auto SourcePrg =
    "def id(var ~Int) do\n"
    "  ret 0;\n"
    "end";
  EXPECT_LL(SourcePrg, "define i32 @id(i32)", "ret i32 0");
}

TEST(CodeGen, FunctionArgBinding)
{
  auto SourcePrg =
    "def id(var ~Int) do\n"
    "  ret var ~Int;\n"
    "end";
  auto ExpectedLL =
    "define i32 @id(i32) gc \"rhgc\" {\n"
    "entry:\n"
    "  ret i32 %0\n"
    "}\n";
  EXPECT_LL(SourcePrg, ExpectedLL);
}

TEST(CodeGen, BindPropagation) {
  auto SourcePrg =
    "def bsym do"
    "  Sym = 3;\n"
    "  ret Sym;\n"
    "end";
  auto ExpectedLL =
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
  auto SourcePrg =
    "def foom do\n"
    "  ret 2;\n"
    "end\n"
    "def main do\n"
    "  ret $ foom ();\n"
    "end";
  auto ExpectedLL =
    "ret i32 %foom";
  EXPECT_LL(SourcePrg, ExpectedLL);
}

TEST(CodeGen, VoidLowering)
{
  auto SourcePrg =
    "def id do\n"
    "  Var = 3; ret ();\n"
    "end";
  auto ExpectedLL =
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
  auto SourcePrg =
    "def foo(a ~Int b ~Int) do\n"
    "  ret $ a + b;\n"
    "end\n"
    "def main do\n"
    "  ret $ foo 3 2;\n"
    "end";
  auto ExpectedLL =
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
  auto SourcePrg =
    "def boom(addfn ~Function(Int -> Int -> Int)) do\n"
    "  ret $ addfn 2 4;\n"
    "end";
  auto ExpectedLL =
    "define i32 @boom(i32 (i32, i32)*)";
  EXPECT_LL(SourcePrg, ExpectedLL);
}
