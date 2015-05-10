#include "rhine/Diagnostic.h"
#include "rhine/StdCapture.h"
#include "gtest/gtest.h"

void EXPECT_PARSE_PP(std::string SourcePrg, std::string *ExpectedErr = nullptr,
                     std::string *ExpectedPP = nullptr)
{
  auto Source = rhine::parseCodeGenString(SourcePrg, Scratch);
  auto Actual = Scratch.str();
  EXPECT_PRED_FORMAT2(::testing::IsSubstring, ExpectedErr->c_str(),
                      Actual.c_str());
}

TEST(CodeGen, FunctionArgBinding)
{
  std::string SourcePrg =
    "defun id [var ~Int] var ~Int;\n";
  std::string ExpectedPP =
    "define i32 @id(i32) {\n"
    "entry:\n"
    "  ret i32 %0\n"
    "}\n";
  EXPECT_PARSE_PP(SourcePrg, nullptr, &ExpectedPP);
}
