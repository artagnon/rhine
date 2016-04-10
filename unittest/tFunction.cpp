#include "rhine/Util/TestUtil.hpp"
#include "gtest/gtest.h"

using namespace rhine;

TEST(Function, MultipleDefun) {
  auto SourcePrg = "def foo do\n"
                   "  ret 2;\n"
                   "end\n"
                   "def bar do\n"
                   "   ret 3;\n"
                   "end";
  EXPECT_LL(SourcePrg, "define i32 @foo()", "define i32 @bar()");
}

TEST(Function, SimpleCall) {
  auto SourcePrg = "def printid (var String) do\n"
                   "  print var;\n"
                   "end\n"
                   "def main do\n"
                   "  printid '21';\n"
                   "end";
  EXPECT_OUTPUT(SourcePrg, "21");
}

TEST(Function, MultipleArguments) {
  auto SourcePrg = "def foo(a Int, b Int) do\n"
                   "  ret $ a + b;\n"
                   "end\n"
                   "def main do\n"
                   "  ret $ foo 3 2;\n"
                   "end";
  EXPECT_LL(SourcePrg, "%2 = add i32 %0, %1");
}

TEST(Function, VoidArgument) {
  auto SourcePrg = "def printSomething do\n"
                   "  print '27';\n"
                   "end\n"
                   "def main do\n"
                   "  printSomething ();\n"
                   "end";
  EXPECT_LL(SourcePrg, "call void @printSomething()");
  EXPECT_OUTPUT(SourcePrg, "27");
}

TEST(Function, FunctionPointerArgument) {
  auto SourcePrg = "def boom(addfn Function(Int -> Int -> Int)) do\n"
                   "  ret $ addfn 2 4;\n"
                   "end";
  EXPECT_LL(SourcePrg, "define i32 @boom(i32 (i32, i32)*)");
}

TEST(Function, VoidReturn) {
  auto SourcePrg = "def id do\n"
                   "  ret ();\n"
                   "end";
  EXPECT_LL(SourcePrg, "ret void");
}
