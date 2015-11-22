#include "rhine/Util/TestUtil.h"
#include "gtest/gtest.h"

using namespace rhine;

TEST(CodeGen, TypeAnnotation) {
  auto SourcePrg = "def id(var ~Int) do\n"
                   "  ret 0;\n"
                   "end";
  EXPECT_LL(SourcePrg, "define i32 @id(i32)", "ret i32 0");
}
