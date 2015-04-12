#include "rhine/IR.h"
#include "rhine/Support.h"
#include "gtest/gtest.h"

#include <regex>

void EXPECT_PARSE_PP(std::string SourcePrg, std::string *ExpectedErr = nullptr,
                     std::string *ExpectedPP = nullptr)
{
  std::regex AnsiColorRe("\\x1b\\[[0-9;]*m");
  std::ostringstream Scratch;
  auto Source = rhine::parseCodeGenString(SourcePrg, Scratch);
  auto Actual = Scratch.str();
  auto CleanActualErr = std::regex_replace(Actual, AnsiColorRe, "");
  if (ExpectedErr) {
    ASSERT_EQ(ExpectedPP, nullptr);
    EXPECT_PRED_FORMAT2(::testing::IsSubstring, ExpectedErr->c_str(),
                        CleanActualErr.c_str());
  } else {
    ASSERT_NE(ExpectedPP, nullptr);
    ASSERT_STREQ("", CleanActualErr.c_str());
    EXPECT_PRED_FORMAT2(::testing::IsSubstring, ExpectedPP->c_str(),
                        Source.c_str());
  }
}

TEST(Parse, BareDefun)
{
  std::string SourcePrg = "defun foo []";
  std::string ExpectedErr = "string stream:1:13: error: syntax error";
  EXPECT_PARSE_PP(SourcePrg, &ExpectedErr);
}

TEST(CodeGen, DefunStm)
{
  std::string SourcePrg = "defun foo [] 3 + 2;";
  std::string ExpectedPP =
    "define i32 @foo() {\n"
    "entry:\n"
    "  ret i32 5\n"
    "}\n";
  EXPECT_PARSE_PP(SourcePrg, nullptr, &ExpectedPP);
}

TEST(CodeGen, DefunCompoundStm)
{
  std::string SourcePrg =
    "defun foo []\n"
    "{\n"
    "  3 + 2;\n"
    "  4 + 5;\n"
    "}";
  std::string ExpectedPP =
    "define i32 @foo() {\n"
    "entry:\n"
    "  ret i32 9\n"
    "}\n";
  EXPECT_PARSE_PP(SourcePrg, nullptr, &ExpectedPP);
}

TEST(CodeGen, MultipleDefun)
{
  std::string SourcePrg =
    "defun foo [] 2;\n"
    "defun bar [] 3;\n";
  std::string ExpectedPP =
    "define i32 @foo() {\n"
    "entry:\n"
    "  ret i32 2\n"
    "}\n\n"
    "define i32 @bar() {\n"
    "entry:\n"
    "  ret i32 3\n"
    "}\n";
  EXPECT_PARSE_PP(SourcePrg, nullptr, &ExpectedPP);
}

TEST(CodeGen, FunctionCall)
{
  std::string SourcePrg = "defun foom [] printf \"43\";";
  std::string ExpectedPP =
    "call i32 (i8*, ...)* @printf";
  EXPECT_PARSE_PP(SourcePrg, nullptr, &ExpectedPP);
}

TEST(CodeGen, TypeAnnotation)
{
  std::string SourcePrg =
    "defun id [var ~ Int] 0;\n";
  std::string ExpectedPP =
    "define i32 @id(i32) {\n"
    "entry:\n"
    "  ret i32 0\n"
    "}\n";
  EXPECT_PARSE_PP(SourcePrg, nullptr, &ExpectedPP);
}

TEST(CodeGen, DISABLED_FunctionArgBinding)
{
  std::string SourcePrg =
    "defun id [var ~ Int] var ~ Int;\n";
  std::string ExpectedPP =
    "define i32 @id(i32 %var) {\n"
    "entry:\n"
    "  ret i32 %var\n"
    "}\n";
  EXPECT_PARSE_PP(SourcePrg, nullptr, &ExpectedPP);
}
