#include "rhine/TestUtil.h"

#include <string>

namespace rhine {
void EXPECT_TRANSFORM_PP(std::string &SourcePrg, std::string &ExpectedPP)
{
  std::ostringstream Scratch;
  auto Pf = ParseFacade(SourcePrg, Scratch);
  auto Source = Pf.parseAction(ParseSource::STRING, PostParseAction::IR);
  auto Err = Scratch.str();
  EXPECT_PRED_FORMAT2(::testing::IsSubstring, ExpectedPP.c_str(),
                      Source.c_str());
}

void EXPECT_PARSE_PP(std::string SourcePrg, std::string ExpectedPP)
{
  std::ostringstream Scratch;
  auto Pf = ParseFacade(SourcePrg, Scratch);
  auto Source = Pf.parseAction(ParseSource::STRING, PostParseAction::LL);
  auto ActualErr = Scratch.str();
  ASSERT_STREQ("", ActualErr.c_str());
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
}
