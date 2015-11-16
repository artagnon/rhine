#include "gtest/gtest.h"
#include "gtest/internal/gtest-port.h"
#include "rhine/Toplevel/ParseFacade.h"
#include "rhine/Util/TestUtil.h"

#include <string>

namespace rhine {
void EXPECT_IR(std::string &SourcePrg, std::string &ExpectedIR)
{
  ParseFacade Pf(SourcePrg);
  auto Source = Pf.parseAction(ParseSource::STRING, PostParseAction::IR);
  EXPECT_PRED_FORMAT2(::testing::IsSubstring, ExpectedIR.c_str(),
                      Source.c_str());
}

void EXPECT_SUCCESSFUL_PARSE(std::string &SourcePrg)
{
  std::string ExpectedIR = "";
  return EXPECT_IR(SourcePrg, ExpectedIR);
}

void EXPECT_LL(std::string &SourcePrg, std::string &ExpectedLL)
{
  ParseFacade Pf(SourcePrg);
  auto Source = Pf.parseAction(ParseSource::STRING, PostParseAction::LL);
  EXPECT_PRED_FORMAT2(::testing::IsSubstring, ExpectedLL.c_str(),
                      Source.c_str());
}

void EXPECT_OUTPUT(std::string &SourcePrg, std::string &ExpectedOut)
{
  ParseFacade Pf(SourcePrg);
  auto Handle = Pf.jitAction(ParseSource::STRING, PostParseAction::LL);
  testing::internal::CaptureStdout();
  Handle();
  std::string ActualOut = testing::internal::GetCapturedStdout();
  EXPECT_STREQ(ExpectedOut.c_str(), ActualOut.c_str());
}

void EXPECT_COMPILE_DEATH(std::string &SourcePrg, std::string &ExpectedErr)
{
  ParseFacade Pf(SourcePrg);
  EXPECT_DEATH(
      Pf.parseAction(ParseSource::STRING, PostParseAction::LL), ExpectedErr);
}
}
