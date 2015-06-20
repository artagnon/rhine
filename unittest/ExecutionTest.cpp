#include "rhine/TestUtil.h"
#include "gtest/gtest.h"

using namespace rhine;

TEST(Execution, FunctionArgument) {
  std::string SourcePrg =
    "def printid [var ~ String] {\n"
    "  print var;\n"
    "}\n"
    "def main [] {\n"
    "  printid '21';\n"
    "}";
  std::string ExpectedOut = "21";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}

TEST(Execution, BasicFunctionPointer) {
  std::string SourcePrg =
    "def callee [] {\n"
    "  3;\n"
    "}\n"
    "def main [] {\n"
    "  callee;\n"
    "}";
  std::string ExpectedOut = "";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}

TEST(Execution, ExternalsFunctionPointer) {
  std::string SourcePrg =
    "def main [] {\n"
    "  malloc;\n"
    "}";
  std::string ExpectedOut = "";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}

TEST(Execution, DollarOperator) {
  std::string SourcePrg =
    "def foo [x ~Int] '2';\n"
    "def main [] print $ foo 3;";
  std::string ExpectedOut = "2";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}

TEST(Execution, FunctionPointer)
{
  std::string SourcePrg =
    "def bar [addfn ~Fn(Int -> Int -> Int)] addfn 2 4;\n"
    "def addCandidate [a ~Int b ~Int] a + b;\n"
    "def main [] bar addCandidate;";
  std::string ExpectedOut = "";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}

TEST(Execution, ToString)
{
  std::string SourcePrg =
    "def main [] print $ toString 2;";
  std::string ExpectedOut = "2";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}

TEST(Execution, VoidArgument) {
  std::string SourcePrg =
    "def printSomething [] {\n"
    "  print '27';\n"
    "}\n"
    "def main [] {\n"
    "  printSomething ();\n"
    "}";
  std::string ExpectedOut = "27";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}

TEST(Execution, DISABLED_FunctionPointerSwitch)
{
  std::string SourcePrg =
    "def bar [ArithFn ~Fn(Int -> Int -> Int)] ArithFn 2 4;\n"
    "def addCandidate [A ~Int B ~Int] A + B;\n"
    "def subCandidate [A ~Int B ~Int] A - B;\n"
    "def main [] if(0) bar addCandidate; else bar subCandidate;";
  std::string ExpectedOut = "";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}
