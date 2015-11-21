#include "rhine/Util/TestUtil.h"
#include "gtest/gtest.h"

using namespace rhine;

TEST(CodeGen, TypeAnnotation) {
  auto SourcePrg = "def id(var ~Int) do\n"
                   "  ret 0;\n"
                   "end";
  EXPECT_LL(SourcePrg, "define i32 @id(i32)", "ret i32 0");
}

TEST(CodeGen, FunctionArgBinding) {
  auto SourcePrg = "def id(var ~Int) do\n"
                   "  ret var ~Int;\n"
                   "end";
  EXPECT_LL(SourcePrg, "define i32 @id(i32)", "ret i32 %0");
}

TEST(CodeGen, BindPropagation) {
  auto SourcePrg = "def bsym do"
                   "  Sym = 3;\n"
                   "  ret Sym;\n"
                   "end";
  EXPECT_LL(SourcePrg, "bitcast i8* %Alloc to i32*", "store i32 3, i32* %0",
            "load i32, i32* %0");
}

TEST(CodeGen, FunctionCall) {
  auto SourcePrg = "def foom do\n"
                   "  ret 2;\n"
                   "end\n"
                   "def main do\n"
                   "  ret $ foom ();\n"
                   "end";
  EXPECT_LL(SourcePrg, "ret i32 %foom");
}

TEST(CodeGen, VoidLowering) {
  auto SourcePrg = "def id do\n"
                   "  ret ();\n"
                   "end";
  EXPECT_LL(SourcePrg, "ret void");
}

TEST(CodeGen, MultipleArguments) {
  auto SourcePrg = "def foo(a ~Int b ~Int) do\n"
                   "  ret $ a + b;\n"
                   "end\n"
                   "def main do\n"
                   "  ret $ foo 3 2;\n"
                   "end";
  EXPECT_LL(SourcePrg, "%2 = add i32 %0, %1");
}

TEST(CodeGen, ArgumentTypesChaining) {
  auto SourcePrg = "def boom(addfn ~Function(Int -> Int -> Int)) do\n"
                   "  ret $ addfn 2 4;\n"
                   "end";
  EXPECT_LL(SourcePrg, "define i32 @boom(i32 (i32, i32)*)");
}
