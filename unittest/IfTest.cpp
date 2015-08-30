#include "rhine/TestUtil.h"
#include "gtest/gtest.h"

using namespace rhine;

TEST(If, BasicCodeGen)
{
  std::string SourcePrg =
    "def main [] { if (false) 2; else 3; }";
  std::string ExpectedPP =
    "define void @main() gc \"rhgc\" {\n"
    "entry:\n"
    "  br i1 false, label %true, label %false\n\n"
    "true:                                             ; preds = %entry\n"
    "  br label %merge\n\n"
    "false:                                            ; preds = %entry\n"
    "  br label %merge\n\n"
    "merge:                                            ; preds = %false, %true\n"
    "  %iftmp = phi i32 [ 2, %true ], [ 3, %false ]\n"
    "  ret void\n"
    "}";
  EXPECT_PARSE_PP(SourcePrg, ExpectedPP);
}

TEST(If, BasicExecution)
{
  std::string SourcePrg =
    "def main [] { if (false) print '2'; else print '3'; }";
  std::string ExpectedOut = "3";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}

TEST(If, DISABLED_CondAssign)
{
  std::string SourcePrg =
    "def main [] {\n"
    "  Handle = 0;\n"
    "  if (false) Handle = 2;\n"
    "  else Handle = 3;\n"
    "  ret Handle;\n"
    "}";
  std::string ExpectedOut = "3";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}
