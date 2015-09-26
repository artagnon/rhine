#include "rhine/Util/TestUtil.h"
#include "gtest/gtest.h"

using namespace rhine;

TEST(Scope, CrossFunctionNameDisambiguation)
{
  std::string SourcePrg =
    "def bar(arithFn ~Function(Int -> Int -> Int)) do\n"
    "  print $ arithFn 2 4;\n"
    "end\n"
    "def addCandidate(A ~Int B ~Int) do\n"
    "  ret $ A + B;\n"
    "end\n"
    "def subCandidate(A ~Int B ~Int) do\n"
    "  ret $ A - B;\n"
    "end\n"
    "def main() do\n"
    "  if false do bar addCandidate; else bar subCandidate; end\n"
    "end";
  std::string ExpectedOut = "-2";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}
