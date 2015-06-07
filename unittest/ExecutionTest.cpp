#include "rhine/Toplevel/ParseFacade.h"
#include "gtest/gtest.h"
#include "gtest/internal/gtest-port.h"

using namespace rhine;

void EXPECT_OUTPUT(std::string &SourcePrg, std::string &ExpectedOut)
{
  auto Pf = ParseFacade(SourcePrg);
  auto Handle = Pf.jitAction(ParseSource::STRING, PostParseAction::LL);
  testing::internal::CaptureStdout();
  Handle();
  std::string ActualOut = testing::internal::GetCapturedStdout();
  EXPECT_STREQ(ExpectedOut.c_str(), ActualOut.c_str());
}

TEST(Execution, PrintfBasic) {
  std::string SourcePrg =
    "def main [] {\n"
    "  println \"43\";\n"
    "}";
  std::string ExpectedOut = "43";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}

TEST(Execution, FunctionArgument) {
  std::string SourcePrg =
    "def printid [var ~ String] {\n"
    "  println var;\n"
    "}\n"
    "def main [] {\n"
    "  printid '21';\n"
    "}";
  std::string ExpectedOut = "21";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}

TEST(Execution, MallocBasic) {
  std::string SourcePrg =
    "def main [] {\n"
    "  malloc 8;\n"
    "  println '3';\n"
    "}";
  std::string ExpectedOut = "3";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}

TEST(Execution, BasicFunctionPointer) {
  std::string SourcePrg =
    "def callee [] {\n"
    "  3;\n"
    "}\n"
    "def main [] {\n"
    "  callee;\n"
    "}";
  std::string ExpectedOut = "";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}

TEST(Execution, ExternalsFunctionPointer) {
  std::string SourcePrg =
    "def main [] {\n"
    "  malloc;\n"
    "}";
  std::string ExpectedOut = "";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}

TEST(Execution, DollarOperator) {
  std::string SourcePrg =
    "def foo [x ~Int] '2';\n"
    "def main [] println $ foo 3;";
  std::string ExpectedOut = "2";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}

TEST(Execution, TyCoerceIntToStr) {
  std::string SourcePrg =
    "def main [] {\n"
    "  println 62;\n"
    "}";
  std::string ExpectedOut = "62";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}

TEST(Execution, FunctionPointer)
{
  std::string SourcePrg =
    "def bar [addfn ~Fn(Int -> Int -> Int)] addfn 2 4;\n"
    "def addCandidate [a ~Int b ~Int] a + b;\n"
    "def main [] bar addCandidate;";
  std::string ExpectedOut = "";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}

TEST(Execution, ToString)
{
  std::string SourcePrg =
    "def main [] println $ toString 2;";
  std::string ExpectedOut = "2";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}
