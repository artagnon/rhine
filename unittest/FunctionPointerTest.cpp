#include "rhine/TestUtil.h"
#include "gtest/gtest.h"

using namespace rhine;

TEST(FunctionPointer, PassPrint)
{
  std::string SourcePrg =
    "def bar [printfn ~Fn(String -> & -> ())] printfn '12';\n"
    "def main [] bar print;";
  std::string ExpectedPP =
    "define void @bar(void (i8*, ...)*) {\n"
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
    "def addCandidate [A ~Int B ~Int] A + B;\n"
    "def main [] bar addCandidate;";
  std::string ExpectedOut = "6";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}

TEST(FunctionPointer, DISABLED_CondAssign)
{
  std::string SourcePrg =
    "def bar [ArithFn ~Fn(Int -> Int -> Int)] ArithFn 2 4;\n"
    "def addCandidate [A ~Int B ~Int] A + B;\n"
    "def subCandidate [A ~Int B ~Int] A - B;\n"
    "def main [] if(false) bar addCandidate; else bar subCandidate;";
  std::string ExpectedOut = "";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}
