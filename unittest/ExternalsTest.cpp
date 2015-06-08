#include "rhine/TestUtil.h"
#include "gtest/gtest.h"

using namespace rhine;

TEST(Externals, PrintCG)
{
  std::string SourcePrg = "def foom [] print '43';";
  std::string ExpectedPP =
    "call void (i8*, ...) @std_Void_print__String";
  EXPECT_PARSE_PP(SourcePrg, ExpectedPP);
}

TEST(CodeGen, MallocCG) {
  std::string SourcePrg =
    "def mallocCall [] {\n"
    "  malloc 8;\n"
    "}";
  std::string ExpectedPP =
    "call i8* @malloc(i64 8)";
  EXPECT_PARSE_PP(SourcePrg, ExpectedPP);
}

TEST(Externals, ExternalsCaching) {
  std::string SourcePrg =
    "def compside [] {"
    "print 'foom';\n"
    "print 'baz';\n"
    "}";
  std::string ExpectedPP =
    "  call void (i8*, ...) @std_Void_print__String"
    "(i8* getelementptr inbounds ([5 x i8], [5 x i8]* @0, i32 0, i32 0))\n"
    "  call void (i8*, ...) @std_Void_print__String"
    "(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @1, i32 0, i32 0))";
  EXPECT_PARSE_PP(SourcePrg, ExpectedPP);
}

TEST(Externals, PrintExec) {
  std::string SourcePrg =
    "def main [] {\n"
    "  print '43';\n"
    "}";
  std::string ExpectedOut = "43";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}

TEST(Externals, MallocExec) {
  std::string SourcePrg =
    "def main [] {\n"
    "  malloc 8;\n"
    "  print '3';\n"
    "}";
  std::string ExpectedOut = "3";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}

TEST(Externals, PrintfPercentChars) {
  std::string SourcePrg =
    "def main [] {\n"
    "  print '%43';\n"
    "}";
  std::string ExpectedOut = "%43";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}
