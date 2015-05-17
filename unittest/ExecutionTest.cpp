#include "rhine/IR.h"
#include "rhine/Support.h"
#include "rhine/Toplevel.h"
#include "rhine/StdCapture.h"
#include "gtest/gtest.h"

void EXPECT_OUTPUT(std::string SourcePrg, std::string ExpectedOut)
{
  auto Handle = rhine::jitFacade(SourcePrg, false, true);
  auto CaptureH = StdCapture();
  CaptureH.BeginCapture();
  Handle();
  CaptureH.EndCapture();
  std::string ActualOut = CaptureH.GetCapture();
  EXPECT_STREQ(ExpectedOut.c_str(), ActualOut.c_str());
}

TEST(Execution, PrintfBasic) {
  std::string SourcePrg =
    "defun main [] {"
    "  printf \"43\";"
    "}";
  std::string ExpectedOut = "43";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}

TEST(Execution, FunctionArgument) {
  std::string SourcePrg =
    "defun printid [var ~ String] {"
    "  printf var;"
    "}"
    "defun main [] {"
    "  printid \"21\";"
    "}";
  std::string ExpectedOut = "21";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}

TEST(Execution, MallocBasic) {
  std::string SourcePrg =
    "defun main [] {\n"
    "  malloc 8;\n"
    "  3;\n"
    "}";
  std::string ExpectedOut = "";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}
