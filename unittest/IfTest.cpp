#include "rhine/TestUtil.h"
#include "gtest/gtest.h"

using namespace rhine;

TEST(If, BasicCodeGen)
{
  std::string SourcePrg =
    "def main [] { if (false) 2; else 3; }";
  std::string ExpectedPP =
    "define i32 @main() {\n"
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
