#include "rhine/Util/TestUtil.h"
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

TEST(Function, FunctionArgument) {
  auto SourcePrg = "def printid (var ~String) do\n"
                   "  print var;\n"
                   "end\n"
                   "def main do\n"
                   "  printid '21';\n"
                   "end";
  EXPECT_OUTPUT(SourcePrg, "21");
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

TEST(CodeGen, VoidReturn) {
  auto SourcePrg = "def id do\n"
                   "  ret ();\n"
                   "end";
  EXPECT_LL(SourcePrg, "ret void");
}
