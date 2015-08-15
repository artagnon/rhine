#include "rhine/TestUtil.h"
#include "gtest/gtest.h"

using namespace rhine;

TEST(If, DISABLED_BasicCodeGen)
{
  std::string SourcePrg =
    "def main [] { if (false) 2; else 3; }";
  std::string ExpectedPP =
    "define i32 @main() gc \"rhgc\" {\n"
    "entry:\n"
    "  br i1 false, label %true, label %false\n\n"
    "true:                                             ; preds = %entry\n"
    "  br label %merge\n\n"
    "false:                                            ; preds = %entry\n"
    "  br label %merge\n\n"
    "merge:                                            ; preds = %false, %true\n"
    "  %iftmp = phi i32 [ 2, %true ], [ 3, %false ]\n"
    "  ret i32 %iftmp\n"
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
