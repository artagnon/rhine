#include "rhine/TestUtil.h"

#include <string>

namespace rhine {
void EXPECT_IR(std::string &SourcePrg, std::string &ExpectedPP)
{
  auto Pf = ParseFacade(SourcePrg);
  auto Source = Pf.parseAction(ParseSource::STRING, PostParseAction::IR);
  EXPECT_PRED_FORMAT2(::testing::IsSubstring, ExpectedPP.c_str(),
                      Source.c_str());
}

void EXPECT_LL(std::string &SourcePrg, std::string &ExpectedPP)
{
  auto Pf = ParseFacade(SourcePrg);
  auto Source = Pf.parseAction(ParseSource::STRING, PostParseAction::LL);
  EXPECT_PRED_FORMAT2(::testing::IsSubstring, ExpectedPP.c_str(),
                      Source.c_str());
}

void EXPECT_OUTPUT(std::string &SourcePrg, std::string &ExpectedOut)
{
  auto Pf = ParseFacade(SourcePrg);
  auto Handle = Pf.jitAction(ParseSource::STRING, PostParseAction::LL);
  testing::internal::CaptureStdout();
  Handle();
  std::string ActualOut = testing::internal::GetCapturedStdout();
  EXPECT_STREQ(ExpectedOut.c_str(), ActualOut.c_str());
}

void EXPECT_COMPILE_DEATH(std::string &SourcePrg, std::string &ExpectedErr)
{
  auto Pf = ParseFacade(SourcePrg);
  EXPECT_DEATH(
      Pf.parseAction(ParseSource::STRING, PostParseAction::LL), ExpectedErr);
}
}
