#include "rhine/Util/TestUtil.h"
#include "gtest/gtest.h"

using namespace rhine;

TEST(TyInfer, CallInst) {
  std::string SourcePrg =
    "def mallocCall() do\n"
    "  ret $ malloc 8;\n"
    "end";
  std::string ExpectedPP =
    "define i8* @mallocCall() gc \"rhgc\" {\n"
    "entry:\n"
    "  %malloc = call i8* @std_String_malloc__Int(i64 8)\n"
    "  ret i8* %malloc\n"
    "}\n";
  EXPECT_LL(SourcePrg, ExpectedPP);
}
