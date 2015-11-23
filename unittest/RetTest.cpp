#include "rhine/Util/TestUtil.h"
#include "gtest/gtest.h"

using namespace rhine;

TEST(Ret, Assignment) {
  auto SourcePrg = "def main() do\n"
                   "  ret $ X = 3;\n"
                   "end";
  auto Err = "string stream:2:9: error: cannot return expression of Void type";
  EXPECT_COMPILE_DEATH(SourcePrg, Err);
}

TEST(Ret, VoidFunction) {
  auto SourcePrg = "def foo() do ret () end\n"
                   "def main() do\n"
                   "  ret $ foo ();\n"
                   "end";
  auto Err = "string stream:3:9: error: cannot return expression of Void type";
  EXPECT_COMPILE_DEATH(SourcePrg, Err);
}

TEST(Ret, DISABLED_Ret) {
  auto SourcePrg = "def main() do\n"
                   "  ret $ ret 3;\n"
                   "end";
  auto Err = "string stream:2:7: error: expected 'if' expression";
  EXPECT_COMPILE_DEATH(SourcePrg, Err);
}
