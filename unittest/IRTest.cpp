#include "rhine/Toplevel/ParseFacade.h"
#include "gtest/gtest.h"

using namespace rhine;

void EXPECT_TRANSFORM_PP(std::string &SourcePrg, std::string &ExpectedPP)
{
  std::ostringstream Scratch;
  auto Pf = ParseFacade(SourcePrg, Scratch);
  auto Source = Pf.parseAction(ParseSource::STRING, PostParseAction::IR);
  auto Err = Scratch.str();
  EXPECT_PRED_FORMAT2(::testing::IsSubstring, ExpectedPP.c_str(),
                      Source.c_str());
}

TEST(IR, ConstantInt) {
  std::string SourcePrg = "def foo [] 3;";
  std::string ExpectedPP =
    "foo ~Fn(() -> Int)\n"
    "3 ~Int";
  EXPECT_TRANSFORM_PP(SourcePrg, ExpectedPP);
}

TEST(IR, AddTwoInt)
{
  std::string SourcePrg = "def foo [] 3 + 2;";
  std::string ExpectedPP =
    "foo ~Fn(() -> Int)\n"
    "+ ~Fn(Int -> Int -> Int)\n"
    "3 ~Int\n"
    "2 ~Int";
  EXPECT_TRANSFORM_PP(SourcePrg, ExpectedPP);
}

TEST(IR, ConstantString)
{
  std::string SourcePrg = "def foo [] \"moo!\";";
  std::string ExpectedPP =
    "foo ~Fn(() -> String)\n"
    "\"moo!\" ~String\n";
  EXPECT_TRANSFORM_PP(SourcePrg, ExpectedPP);
}

TEST(IR, TypePropagation)
{
  std::string SourcePrg = "def id [var ~Int] var;\n";
  std::string ExpectedPP =
    "id ~Fn(Int -> Int)\n"
    "var ~Int\n"
    "var ~Int\n";
  EXPECT_TRANSFORM_PP(SourcePrg, ExpectedPP);
}

TEST(IR, BindInst) {
  std::string SourcePrg = "def bsym [] sym = 3;";
  std::string ExpectedPP =
    "bsym ~Fn(() -> ())\n"
    "sym = 3 ~Int";
  EXPECT_TRANSFORM_PP(SourcePrg, ExpectedPP);
}

TEST(IR, BindPropagation) {
  std::string SourcePrg = "def bsym [] {"
    "sym = 3;\n"
    "sym;\n"
    "}";
  std::string ExpectedPP =
    "bsym ~Fn(() -> Int)\n"
    "sym = 3 ~Int\n"
    "sym ~Int";
  EXPECT_TRANSFORM_PP(SourcePrg, ExpectedPP);
}

TEST(IR, Comment) {
  std::string SourcePrg = "def foo []\n"
    "// Strip this out\n"
    "3;";
  std::string ExpectedPP =
    "foo ~Fn(() -> Int)\n"
    "3 ~Int";
  EXPECT_TRANSFORM_PP(SourcePrg, ExpectedPP);
}

TEST(IR, TwoArguments)
{
  std::string SourcePrg = "def foo [a ~Int b ~Int] a + b;";
  std::string ExpectedPP =
    "foo ~Fn(Int -> Int -> Int)\n"
    "a ~Int\n"
    "b ~Int\n"
    "+ ~Fn(Int -> Int -> Int)\n"
    "a ~Int\n"
    "b ~Int";
  EXPECT_TRANSFORM_PP(SourcePrg, ExpectedPP);
}

TEST(IR, TypePropagationCallInst)
{
  std::string SourcePrg = "def id [var ~String] println var;\n";
  std::string ExpectedPP =
    "id ~Fn(String -> Int)\n"
    "var ~String\n"
    "println ~Fn(String -> Int)*\n"
    "var ~String\n";
  EXPECT_TRANSFORM_PP(SourcePrg, ExpectedPP);
}
