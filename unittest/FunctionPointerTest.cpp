#include "rhine/TestUtil.h"
#include "gtest/gtest.h"

using namespace rhine;

TEST(FunctionPointer, BasicCodeGen) {
  std::string SourcePrg =
    "def callee [] {\n"
    "  3;\n"
    "}\n"
    "def caller [] {\n"
    "  ret callee;\n"
    "}";
  std::string ExpectedPP =
    "define i32 ()* @caller() gc \"rhgc\" {\n"
    "entry:\n"
    "  ret i32 ()* @callee\n"
    "}\n";
  EXPECT_PARSE_PP(SourcePrg, ExpectedPP);
}

TEST(FunctionPointer, BasicExecution) {
  std::string SourcePrg =
    "def callee [] {\n"
    "  ret 3;\n"
    "}\n"
    "def main [] {\n"
    "  ret callee;\n"
    "}";
  std::string ExpectedOut = "";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}

TEST(FunctionPointer, DISABLED_Externals) {
  std::string SourcePrg =
    "def main [] {\n"
    "  ret malloc;\n"
    "}";
  std::string ExpectedOut = "";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}

TEST(FunctionPointer, PassPrint)
{
  std::string SourcePrg =
    "def bar [printfn ~Fn(String -> & -> ())] printfn '12';\n"
    "def main [] bar print;";
  std::string ExpectedPP =
    "define void @bar(void (i8*, ...)*) gc \"rhgc\" {\n"
    "entry:\n"
    "  call void (i8*, ...) %0(i8* getelementptr";
  EXPECT_PARSE_PP(SourcePrg, ExpectedPP);
  std::string ExpectedOut = "12";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}

TEST(FunctionPointer, PassCustomFunction)
{
  std::string SourcePrg =
    "def bar [addfn ~Fn(Int -> Int -> Int)] print $ addfn 2 4;\n"
    "def addCandidate [A ~Int B ~Int] ret $ A + B;\n"
    "def main [] bar addCandidate;";
  std::string ExpectedOut = "6";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}

TEST(FunctionPointer, DISABLED_CondAssign)
{
  std::string SourcePrg =
    "def bar [arithFn ~Fn(Int -> Int -> Int)] print $ arithFn 2 4;\n"
    "def addCandidate [A ~Int B ~Int] ret $ A + B;\n"
    "def subCandidate [A ~Int B ~Int] ret $ A - B;\n"
    "def main [] if(false) bar addCandidate; else bar subCandidate;";
  std::string ExpectedOut = "";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}
