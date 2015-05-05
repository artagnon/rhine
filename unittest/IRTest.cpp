#include "rhine/IR.h"
#include "rhine/Support.h"
#include "gtest/gtest.h"

void EXPECT_PARSE_PP(std::string SourcePrg, std::string ExpectedPP)
{
  std::ostringstream Scratch;
  auto Source = rhine::parseTransformIR(SourcePrg, Scratch);
  auto Err = Scratch.str();
  EXPECT_PRED_FORMAT2(::testing::IsSubstring, ExpectedPP.c_str(),
                      Source.c_str());
}

TEST(IR, ConstantInt) {
  auto SourcePrg = "defun foo [] 3;";
  auto ExpectedPP =
    "foo ~Fn(() -> Int)\n"
    "3 ~Int";
  EXPECT_PARSE_PP(SourcePrg, ExpectedPP);
}

TEST(IR, AddTwoInt)
{
  std::string SourcePrg = "defun foo [] 3 + 2;";
  std::string ExpectedPP =
    "foo ~Fn(() -> Int)\n"
    "+ ~Int\n"
    "3 ~Int\n"
    "2 ~Int";
  EXPECT_PARSE_PP(SourcePrg, ExpectedPP);
}

TEST(IR, ConstantString)
{
  std::string SourcePrg = "defun foo [] \"moo!\";";
  std::string ExpectedPP =
    "foo ~Fn(() -> String)\n"
    "\"moo!\" ~String\n";
  EXPECT_PARSE_PP(SourcePrg, ExpectedPP);
}

TEST(IR, TypePropagation)
{
  std::string SourcePrg = "defun id [var ~Int] var;\n";
  std::string ExpectedPP =
    "id ~Fn(Int -> Int)\n"
    "var ~Int\n"
    "var ~Int\n";
  EXPECT_PARSE_PP(SourcePrg, ExpectedPP);
}

TEST(IR, BindInst) {
  auto SourcePrg = "defun bsym [] sym = 3;";
  auto ExpectedPP =
    "bsym ~Fn(() -> ())\n"
    "sym = 3 ~Int";
  EXPECT_PARSE_PP(SourcePrg, ExpectedPP);
}
