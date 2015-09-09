#include "rhine/Util/TestUtil.h"
#include "gtest/gtest.h"

using namespace rhine;

TEST(Execution, FunctionArgument) {
  std::string SourcePrg =
    "def printid [var ~String] {\n"
    "  print var;\n"
    "}\n"
    "def main [] {\n"
    "  printid '21';\n"
    "}";
  std::string ExpectedOut = "21";
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
