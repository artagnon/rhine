#include "rhine/TestUtil.h"
#include "gtest/gtest.h"

using namespace rhine;

TEST(TyInfer, CallInst) {
  std::string SourcePrg =
    "def mallocCall [] {\n"
    "  malloc 8;\n"
    "}";
  std::string ExpectedPP =
    "define i8* @mallocCall() {\n"
    "entry:\n"
    "  %rhv0 = call i8* @std_String_malloc__Int(i64 8)\n"
    "  ret i8* %rhv0\n"
    "}\n";
  EXPECT_PARSE_PP(SourcePrg, ExpectedPP);
}
