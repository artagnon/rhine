#include "rhine/Util/TestUtil.h"
#include "gtest/gtest.h"

using namespace rhine;

TEST(TyInfer, CallInst) {
  auto SourcePrg =
    "def mallocCall do\n"
    "  ret $ malloc 8;\n"
    "end";
  auto ExpectedPP =
    "define i8* @mallocCall() gc \"rhgc\" {\n"
    "entry:\n"
    "  %malloc = call i8* @std_String_malloc__Int(i64 8)\n"
    "  ret i8* %malloc\n"
    "}\n";
  EXPECT_LL(SourcePrg, ExpectedPP);
}

TEST(TyInfer, PropagationFromArgument)
{
  auto SourcePrg =
    "def id(var ~Int) do\n"
    "  ret var;\n"
    "end";
  auto ExpectedLL =
    "define i32 @id(i32) gc \"rhgc\" {\n"
    "entry:\n"
    "  ret i32 %0\n"
    "}\n";
  EXPECT_LL(SourcePrg, ExpectedLL);
}
