#include "rhine/Util/TestUtil.h"
#include "gtest/gtest.h"

using namespace rhine;

TEST(FunctionPointer, BasicCodeGen) {
  std::string SourcePrg =
    "def callee() do\n"
    "  3;\n"
    "end\n"
    "def caller() do\n"
    "  ret callee;\n"
    "end";
  std::string ExpectedPP =
    "define i32 ()* @caller() gc \"rhgc\" {\n"
    "entry:\n"
    "  ret i32 ()* @callee\n"
    "}";
  EXPECT_LL(SourcePrg, ExpectedPP);
}

TEST(FunctionPointer, BasicExecution) {
  std::string SourcePrg =
    "def callee() do\n"
    "  ret 3;\n"
    "end\n"
    "def main() do\n"
    "  ret callee;\n"
    "end";
  std::string ExpectedOut = "";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}

TEST(FunctionPointer, Externals) {
  std::string SourcePrg =
    "def main() do\n"
    "  ret malloc;\n"
    "end";
  std::string ExpectedOut = "";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}

TEST(FunctionPointer, PassPrint)
{
  std::string SourcePrg =
    "def bar(printfn ~Function(String -> & -> Void)) do\n"
    "  printfn '12';\n"
    "end\n"
    "def main() do\n"
    "  bar print;\n"
    "end";
  std::string ExpectedPP =
    "define void @bar(void (i8*, ...)*) gc \"rhgc\" {\n"
    "entry:\n"
    "  call void (i8*, ...) %0(i8* getelementptr";
  EXPECT_LL(SourcePrg, ExpectedPP);
  std::string ExpectedOut = "12";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}

TEST(FunctionPointer, PassCustomFunction)
{
  std::string SourcePrg =
    "def bar(addfn ~Function(Int -> Int -> Int)) do\n"
    "  print $ addfn 2 4;\n"
    "end\n"
    "def addCandidate(A ~Int B ~Int) do\n"
    "  ret $ A + B;\n"
    "end\n"
    "def main() do\n"
    "  bar addCandidate;"
    "end";
  std::string ExpectedOut = "6";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}

TEST(FunctionPointer, CondAssign)
{
  std::string SourcePrg =
    "def bar(arithFn ~Function(Int -> Int -> Int)) do\n"
    "  print $ arithFn 2 4;\n"
    "end\n"
    "def addCandidate(A ~Int B ~Int) do\n"
    "  ret $ A + B;\n"
    "end\n"
    "def subCandidate(C ~Int D ~Int) do\n"
    "  ret $ C - D;\n"
    "end\n"
    "def main() do\n"
    "  if false do bar addCandidate; else bar subCandidate; end\n"
    "end";
  std::string ExpectedOut = "-2";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}
