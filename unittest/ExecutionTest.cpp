#include "rhine/IR.h"
#include "rhine/Support.h"
#include "rhine/Toplevel.h"
#include "gtest/gtest.h"
#include "gtest/internal/gtest-port.h"

void EXPECT_OUTPUT(std::string &SourcePrg, std::string &ExpectedOut)
{
  auto Handle = rhine::jitFacade(SourcePrg, false, true);
  testing::internal::CaptureStdout();
  Handle();
  std::string ActualOut = testing::internal::GetCapturedStdout();
  EXPECT_STREQ(ExpectedOut.c_str(), ActualOut.c_str());
}

TEST(Execution, PrintfBasic) {
  std::string SourcePrg =
    "defun main [] {\n"
    "  printf \"43\";\n"
    "}";
  std::string ExpectedOut = "43";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}

TEST(Execution, FunctionArgument) {
  std::string SourcePrg =
    "defun printid [var ~ String] {\n"
    "  printf var;\n"
    "}\n"
    "defun main [] {\n"
    "  printid \"21\";\n"
    "}";
  std::string ExpectedOut = "21";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}

TEST(Execution, MallocBasic) {
  std::string SourcePrg =
    "defun main [] {\n"
    "  malloc 8;\n"
    "  printf \"3\";\n"
    "}";
  std::string ExpectedOut = "3";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}


TEST(Execution, DISABLED_FunctionPointer) {
  std::string SourcePrg =
    "defun callee [] {\n"
    "  3;\n"
    "}\n"
    "defun main [] {\n"
    "  callee;\n"
    "}";
  std::string ExpectedOut = "";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}

TEST(Execution, DISABLED_ExternalsFunctionPointer) {
  std::string SourcePrg =
    "defun main [] {\n"
    "  malloc;\n"
    "}";
  std::string ExpectedOut = "";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}
