#include "rhine/Util/TestUtil.h"
#include "gtest/gtest.h"

using namespace rhine;

TEST(TyInfer, CallInst) {
  auto SourcePrg = "def mallocCall do\n"
                   "  ret $ malloc 8;\n"
                   "end";
  EXPECT_LL(SourcePrg, "define i8* @mallocCall()",
            "%malloc = call i8* @std_String_malloc__Int(i64 8)",
            "ret i8* %malloc");
}

TEST(TyInfer, PropagationFromArgument) {
  auto SourcePrg = "def id(var ~Int) do\n"
                   "  ret var;\n"
                   "end";
  EXPECT_LL(SourcePrg, "define i32 @id(i32)", "ret i32 %0");
}
