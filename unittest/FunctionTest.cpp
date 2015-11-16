#include "rhine/Util/TestUtil.h"
#include "gtest/gtest.h"

using namespace rhine;

TEST(Function, MultipleDefun)
{
  auto SourcePrg =
    "def foo do\n"
    "  ret 2;\n"
    "end\n"
    "def bar do\n"
    "   ret 3;\n"
    "end";
  EXPECT_LL(SourcePrg, "define i32 @foo()", "define i32 @bar()");
}

TEST(Function, FunctionArgument) {
  auto SourcePrg =
    "def printid [var ~String] {\n"
    "  print var;\n"
    "}\n"
    "def main [] {\n"
    "  printid '21';\n"
    "}";
  auto ExpectedOut = "21";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}

TEST(Function, VoidArgument) {
  auto SourcePrg =
    "def printSomething [] {\n"
    "  print '27';\n"
    "}\n"
    "def main [] {\n"
    "  printSomething ();\n"
    "}";
  auto ExpectedOut = "27";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}
