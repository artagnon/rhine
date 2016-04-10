#include "rhine/Util/TestUtil.hpp"
#include "gtest/gtest.h"

using namespace rhine;

/// Stress testing passing around and invoking function pointers.

TEST(FunctionPointer, Ret) {
  auto SourcePrg = "def callee() do\n"
                   "  ret 3;\n"
                   "end\n"
                   "def main() do\n"
                   "  ret callee;\n"
                   "end";
  EXPECT_LL(SourcePrg, "define i32 ()* @main()", "ret i32 ()* @callee");
  EXPECT_OUTPUT(SourcePrg, "");
}

TEST(FunctionPointer, Externals) {
  auto SourcePrg = "def main() do\n"
                   "  ret malloc;\n"
                   "end";
  auto ExpectedOut = "";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}

TEST(FunctionPointer, PassPrint) {
  auto SourcePrg = "def bar(printfn Function(String -> & -> Void)) do\n"
                   "  printfn '12';\n"
                   "end\n"
                   "def main() do\n"
                   "  bar print;\n"
                   "end";
  EXPECT_LL(SourcePrg, "define void @bar(void (i8*, ...)*)",
            "call void (i8*, ...) %0(i8* getelementptr");
  auto ExpectedOut = "12";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}

TEST(FunctionPointer, PassCustomFunction) {
  auto SourcePrg = "def bar(addfn Function(Int -> Int -> Int)) do\n"
                   "  print $ addfn 2 4;\n"
                   "end\n"
                   "def addCandidate(A Int, B Int) do\n"
                   "  ret $ A + B;\n"
                   "end\n"
                   "def main() do\n"
                   "  bar addCandidate;"
                   "end";
  auto ExpectedOut = "6";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}

TEST(FunctionPointer, CondAssign) {
  auto SourcePrg =
      "def bar(arithFn Function(Int -> Int -> Int)) do\n"
      "  print $ arithFn 2 4;\n"
      "end\n"
      "def addCandidate(A Int, B Int) do\n"
      "  ret $ A + B;\n"
      "end\n"
      "def subCandidate(C Int, D Int) do\n"
      "  ret $ C - D;\n"
      "end\n"
      "def main() do\n"
      "  if false do bar addCandidate; else bar subCandidate; end\n"
      "end";
  auto ExpectedOut = "-2";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}

TEST(FunctionPointer, DISABLED_UseRetValue) {
  auto SourcePrg = "def bar do\n"
                   "  ret print\n"
                   "end\n"
                   "def main do\n"
                   "  bar () $ 34;\n"
                   "end";
  auto ExpectedOut = "34";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}

TEST(FunctionPointer, DISABLED_VoidArgument_Transparency) {
  auto SourcePrg = "def bar do\n"
                   "  ret print\n"
                   "end\n"
                   "def main do\n"
                   "  bar 34;\n"
                   "end";
  auto ExpectedOut = "34";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}

TEST(FunctionPointer, DISABLED_Chain) {
  auto SourcePrg = "def bar(printfn "
                   "Function(Void -> Function(String -> & -> Void))) do\n"
                   "  printfn 2\n"
                   "end\n"
                   "def printCandidate() do\n"
                   "  ret println;\n"
                   "end\n"
                   "def main() do\n"
                   "  bar printCandidate\n"
                   "end";
  auto ExpectedOut = "6";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}
