#include "rhine/Util/TestUtil.hpp"
#include "gtest/gtest.h"

using namespace rhine;

/// Stress the functionality of the dollar ($) operator.

TEST(DollarOperator, Bare) {
  auto SourcePrg = "def main() do\n"
                   "  $ 3;\n"
                   "end";
  auto Err = "string stream:2:3: error: expecting a single statement";
  EXPECT_COMPILE_DEATH(SourcePrg, Err);
}

TEST(DollarOperator, Redundant) {
  auto SourcePrg = "def main() do\n"
                   "  print $ '3';\n"
                   "end";
  EXPECT_LL(SourcePrg, "call void (i8*, ...) @std_Void_print__String(");
  EXPECT_OUTPUT(SourcePrg, "3");
}

TEST(DollarOperator, WithFunction) {
  auto SourcePrg = "def foo(x Int) do\n"
                   "  ret '2';\n"
                   "end\n"
                   "def main() do\n"
                   "  print $ foo 3;\n"
                   "end";
  EXPECT_LL(SourcePrg, "%foo = call i8* @foo(i32 3)",
            "call void (i8*, ...) @std_Void_print__String(i8* %foo1)");
  EXPECT_OUTPUT(SourcePrg, "2");
}

TEST(DollarOperator, Ret) {
  auto SourcePrg = "def foo do\n"
                   "  ret $ 3 + 2;\n"
                   "end";
  EXPECT_LL(SourcePrg, "ret i32 5");
}

TEST(DollarOperator, NonInstructionRHS) {
  auto SourcePrg = "def foo(X Int) do\n"
                   "  ret X\n"
                   "end\n"
                   "def main() do\n"
                   "  foo $ 3\n"
                   "end";
  EXPECT_LL(SourcePrg, "%foo = call i32 @foo(i32 3)", "ret void");
}

TEST(DollarOperator, DoubleDollar) {
  auto SourcePrg = "def main() do\n"
                   "  ret $ toString $ 3;\n"
                   "end";
  EXPECT_LL(SourcePrg, "%toString = call i8* @std_String_toString__Int(i32 3)",
            "ret i8* %toString");
}

TEST(DollarOperator, Assignment) {
  auto SourcePrg = "def main() do\n"
                   "  X = $ 3;\n"
                   "end";
  auto Err = "string stream:2:7: error: expected 'if' expression";
  EXPECT_COMPILE_DEATH(SourcePrg, Err);
}

TEST(DollarOperator, NonInstructionLHS) {
  auto SourcePrg = "def foo(X Int, Y Int) do\n"
                   "  ret $ X + Y\n"
                   "end\n"
                   "def main() do\n"
                   "  foo $ 3 $ 4\n"
                   "end";
  EXPECT_COMPILE_DEATH(SourcePrg, "string stream:5:11: error:");
}

TEST(DollarOperator, DISABLED_PartialApplication) {
  auto SourcePrg = "def foo(X Int, Y Int) do\n"
                   "  ret '2';\n"
                   "end\n"
                   "def main() do\n"
                   "  foo 3 $ 4;\n"
                   "end";
  EXPECT_LL(SourcePrg, "%foo = call i8* @foo(i32 3, i32 4)",
            "call void (i8*, ...) @std_Void_print__String(i8* %foo1)");
  EXPECT_OUTPUT(SourcePrg, "2");
}
