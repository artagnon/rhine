#include "rhine/Util/TestUtil.hpp"
#include "gtest/gtest.h"

using namespace rhine;

TEST(Lambda, BasicCodeGen) {
  auto SourcePrg = "def foo do\n"
                   "  Bfunc = fn x ~Int -> ret x; end;\n"
                   "end";
  auto ExpectedPP = "  %Alloc = call i8* @std_String_malloc__Int(i64 8)\n"
                    "  %0 = bitcast i8* %Alloc to i32 (i32)**\n"
                    "  store i32 (i32)* @lambda, i32 (i32)** %0";
  EXPECT_LL(SourcePrg, "define i32 @lambda(i32)", "ret i32 %0", ExpectedPP);
}

TEST(Lambda, BasicExecution) {
  auto SourcePrg = "def main do\n"
                   "  Bfunc = fn x ~Int -> ret x; end;\n"
                   "  print $ Bfunc 3;\n"
                   "end";
  auto ExpectedOut = "3";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}
