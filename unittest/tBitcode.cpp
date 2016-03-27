#include "rhine/Util/TestUtil.hpp"
#include "gtest/gtest.h"

#include <fstream>
#include <iomanip>
#include <iostream>

#include "rhine/Toplevel/ParseFacade.hpp"

using namespace rhine;

TEST(Bitcode, BCString)
{
  auto SourcePrg = "def main do ret 2 end";
  ParseFacade Pf(SourcePrg);
  auto BCStr = Pf.parseAction(ParseSource::STRING, PostParseAction::BCString);
  EXPECT_PRED_FORMAT2(::testing::IsSubstring, "BC", BCStr.c_str());
}

TEST(Bitcode, BCWrite)
{
  auto SourcePrg = "def main do ret 2 end";
  ParseFacade Pf(SourcePrg);
  Pf.parseAction(ParseSource::STRING, PostParseAction::BCWrite);
  std::ifstream BCFile("foo.bc");
  EXPECT_TRUE((bool)BCFile);
}

TEST(Bitcode, DISABLED_LinkExecutable)
{
  auto SourcePrg = "def main do ret 2 end";
  ParseFacade Pf(SourcePrg);
  testing::internal::CaptureStderr();
  Pf.parseAction(ParseSource::STRING, PostParseAction::LinkExecutable);
  testing::internal::GetCapturedStderr();
  std::ifstream BCFile("foo");
  auto ExeStream = popen("./foo", "r");
  ASSERT_NE(ExeStream, nullptr);
  ASSERT_EQ(pclose(ExeStream) / 256, 2);
}

TEST(Bitcode, DISABLED_LinkRuntime)
{
  auto SourcePrg = "def main do print '43' end";
  ParseFacade Pf(SourcePrg);
  Pf.parseAction(ParseSource::STRING, PostParseAction::LinkExecutable);
  std::ifstream BCFile("foo");
  char Buf[255];
  FILE *Stream = popen("foo", "r");
  while (!feof(Stream))
    fgets(Buf, 255, Stream);
  pclose(Stream);
  EXPECT_STREQ(Buf, "43");
}
