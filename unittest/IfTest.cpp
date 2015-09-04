#include "rhine/TestUtil.h"
#include "gtest/gtest.h"

using namespace rhine;

TEST(If, BasicCodeGen)
{
  std::string SourcePrg =
    "def main [] { if (false) 2; else 3; }";
  std::string ExpectedLL =
    "define void @main() gc \"rhgc\" {\n"
    "entry:\n"
    "  br i1 false, label %true, label %false\n\n"
    "true:                                             ; preds = %entry\n"
    "  br label %phi\n\n"
    "false:                                            ; preds = %entry\n"
    "  br label %phi\n\n"
    "phi:                                              ; preds = %false, %true\n"
    "  %iftmp = phi i32 [ 2, %true ], [ 3, %false ]\n"
    "  ret void\n"
    "}";
  EXPECT_LL(SourcePrg, ExpectedLL);
}

TEST(If, BasicExecution)
{
  std::string SourcePrg =
    "def main [] { if (false) print '2'; else print '3'; }";
  std::string ExpectedOut = "3";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}

TEST(If, DISABLED_LifeAfterPhi)
{
  std::string SourcePrg =
    "def main [] {\n"
    "  if (true) {\n"
    "     Moo = 2;\n"
    "  } else {\n"
    "     Foo = 4;\n"
    "  }\n"
    "  print 2;\n"
    "}\n";
  std::string ExpectedOut = "2";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}
