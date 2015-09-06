#include "rhine/Util/TestUtil.h"
#include "gtest/gtest.h"

using namespace rhine;

TEST(Externals, Print) {
  std::string SourcePrg =
    "def main [] {\n"
    "  print '43';\n"
    "}";
  std::string ExpectedPP =
    "call void (i8*, ...) @std_Void_print__String";
  EXPECT_LL(SourcePrg, ExpectedPP);
  std::string ExpectedOut = "43";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}

TEST(Externals, ExternalsCaching) {
  std::string SourcePrg =
    "def compside [] {\n"
    "  print 'foom';\n"
    "  print 'baz';\n"
    "}";
  std::string ExpectedPP =
    "  call void (i8*, ...) @std_Void_print__String"
    "(i8* getelementptr inbounds ([5 x i8], [5 x i8]* @0, i32 0, i32 0))\n"
    "  call void (i8*, ...) @std_Void_print__String"
    "(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @1, i32 0, i32 0))";
  EXPECT_LL(SourcePrg, ExpectedPP);
}

TEST(Externals, Malloc) {
  std::string SourcePrg =
    "def main [] {\n"
    "  malloc 8;\n"
    "  print '3';\n"
    "}";
  std::string ExpectedPP =
    "call i8* @std_String_malloc__Int(i64 8)";
  EXPECT_LL(SourcePrg, ExpectedPP);
  std::string ExpectedOut = "3";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}

TEST(Externals, PrintPercentChars) {
  std::string SourcePrg =
    "def main [] {\n"
    "  print '%43';\n"
    "}";
  std::string ExpectedOut = "%43";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}

TEST(Externals, ToString)
{
  std::string SourcePrg =
    "def main [] toString 2;";
  std::string ExpectedPP =
    "%toString = call i8* @std_String_toString__Int(i32 2)";
  EXPECT_LL(SourcePrg, ExpectedPP);
}

TEST(Externals, ToStringExecution)
{
  std::string SourcePrg =
    "def main [] print $ toString 2;";
  std::string ExpectedOut = "2";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}

TEST(Externals, FunctionPointer) {
  std::string SourcePrg =
    "def mallocRet [] {\n"
    "  ret malloc;\n"
    "}";
  std::string ExpectedPP =
    "define i8* (i64)* @mallocRet() gc \"rhgc\" {\n"
    "entry:\n"
    "  ret i8* (i64)* @std_String_malloc__Int\n"
    "}\n";
  EXPECT_LL(SourcePrg, ExpectedPP);
}
