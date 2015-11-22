#include "rhine/Util/TestUtil.h"
#include "gtest/gtest.h"

using namespace rhine;

TEST(DollarOperator, WithFunction) {
  auto SourcePrg = "def foo(x ~Int) do\n"
                   "  ret '2';\n"
                   "end\n"
                   "def main() do\n"
                   "  print $ foo 3;\n"
                   "end";
  auto ExpectedOut = "2";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}

TEST(CodeGen, WithRet) {
  auto SourcePrg = "def foo do\n"
                   "  ret $ 3 + 2;\n"
                   "end";
  EXPECT_LL(SourcePrg, "define i32 @foo()", "ret i32 5");
}
