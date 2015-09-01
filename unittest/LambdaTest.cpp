#include "rhine/TestUtil.h"
#include "gtest/gtest.h"

using namespace rhine;

TEST(Lambda, BasicCodeGen)
{
  std::string SourcePrg =
    "def foo [] Bfunc = \\x ~Int -> ret x;";
  std::string ExpectedPP =
    "define i32 @lambda(i32) gc \"rhgc\" {\n"
    "entry:\n"
    "  ret i32 %0\n"
    "}\n\n"
    "define void @foo() gc \"rhgc\" {\n"
    "entry:\n"
    "  %Alloc = call i8* @std_String_malloc__Int(i64 8)\n"
    "  %0 = bitcast i8* %Alloc to i32 (i32)**\n"
    "  store i32 (i32)* @lambda, i32 (i32)** %0\n"
    "  ret void\n"
    "}";
  EXPECT_LL(SourcePrg, ExpectedPP);
}

TEST(Lambda, BasicExecution)
{
  std::string SourcePrg =
    "def main [] {\n"
    "  Bfunc = \\x ~Int -> ret x;\n"
    "  print $ Bfunc 3;\n"
    "}";
  std::string ExpectedOut = "3";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}
