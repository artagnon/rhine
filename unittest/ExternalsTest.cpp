#include "rhine/Util/TestUtil.h"
#include "gtest/gtest.h"

using namespace rhine;

/// Test functions that should ideally be part of the runtime.

TEST(Externals, Print) {
  auto SourcePrg = "def main() do\n"
                   "  print '43';\n"
                   "end";
  auto ExpectedPP = "call void (i8*, ...) @std_Void_print__String";
  EXPECT_LL(SourcePrg, ExpectedPP);
  auto ExpectedOut = "43";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}

TEST(Externals, ExternalsCaching) {
  auto SourcePrg = "def compside() do\n"
                   "  print 'foom';\n"
                   "  print 'baz';\n"
                   "end";
  auto ExpectedPP =
      "  call void (i8*, ...) @std_Void_print__String"
      "(i8* getelementptr inbounds ([5 x i8], [5 x i8]* @0, i32 0, i32 0))\n"
      "  call void (i8*, ...) @std_Void_print__String"
      "(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @1, i32 0, i32 0))";
  EXPECT_LL(SourcePrg, ExpectedPP);
}

TEST(Externals, Malloc) {
  auto SourcePrg = "def main() do\n"
                   "  malloc 8;\n"
                   "  print '3';\n"
                   "end";
  auto ExpectedPP = "call i8* @std_String_malloc__Int(i64 8)";
  EXPECT_LL(SourcePrg, ExpectedPP);
  auto ExpectedOut = "3";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}

TEST(Externals, PrintPercentChars) {
  auto SourcePrg = "def main() do\n"
                   "  print '%43';\n"
                   "end";
  auto ExpectedOut = "%43";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}

TEST(Externals, ToString) {
  auto SourcePrg = "def main() do\n"
                   "  toString 2;"
                   "end";
  auto ExpectedPP = "%toString = call i8* @std_String_toString__Int(i32 2)";
  EXPECT_LL(SourcePrg, ExpectedPP);
}

TEST(Externals, ToStringExecution) {
  auto SourcePrg = "def main() do\n"
                   "  print $ toString 2;"
                   "end";
  auto ExpectedOut = "2";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}

TEST(Externals, FunctionPointer) {
  auto SourcePrg = "def mallocRet() do\n"
                   "  ret malloc;\n"
                   "end";
  EXPECT_LL(SourcePrg, "define i8* (i64)* @mallocRet()",
            "ret i8* (i64)* @std_String_malloc__Int");
}
