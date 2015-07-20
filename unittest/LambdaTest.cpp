#include "rhine/TestUtil.h"
#include "gtest/gtest.h"

using namespace rhine;

TEST(Lambda, BasicCodeGen)
{
  std::string SourcePrg =
    "def foo [] Bfunc = \\x ~ Int -> x;";
  std::string ExpectedPP =
    "define i32 @lambda(i32) {\n"
    "entry:\n"
    "  ret i32 %0\n"
    "}\n\n"
    "define void @foo() {\n"
    "entry:\n"
    "  %BfuncAlloca = alloca i32 (i32)*\n"
    "  store i32 (i32)* @lambda, i32 (i32)** %BfuncAlloca\n"
    "  ret void\n"
    "}";
  EXPECT_PARSE_PP(SourcePrg, ExpectedPP);
}

TEST(Lambda, DISABLED_BasicExecution)
{
  std::string SourcePrg =
    "def main [] {\n"
    "  Bfunc = \\x ~ Int -> x;\n"
    "  print $ Bfunc 3;\n"
    "}";
  std::string ExpectedOut = "3";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}
