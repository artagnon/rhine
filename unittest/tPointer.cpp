#include "rhine/Util/TestUtil.hpp"
#include "gtest/gtest.h"

using namespace rhine;

TEST(Pointer, AssignToPointer) {
  auto SourcePrg = "def foo do\n"
                   "A = 3\n"
                   "B = A\n"
                   "end";
  EXPECT_LL(SourcePrg, "%ALoad = load i32, i32*",
            "%Alloc = call i8* @std_String_malloc__Int",
            "%Alloc1 = call i8* @std_String_malloc__Int");
}
