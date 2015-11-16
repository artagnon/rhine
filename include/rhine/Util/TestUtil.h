//-*- C++ -*-

#ifndef RHINE_TESTUTIL_H
#define RHINE_TESTUTIL_H

#include "gtest/gtest.h"
#include "gtest/internal/gtest-port.h"
#include "rhine/Toplevel/ParseFacade.h"
#include "rhine/Util/TestUtil.h"

#include <string>

#define CAT_RH(Tok) #Tok

/// Call EXPECT_IR with "" as the second argument.
#define EXPECT_SUCCESSFUL_PARSE(SourcePrg) EXPECT_IR(SourcePrg, std::string(""))

/// All this stuff needs to be in the header because of templates, especially
/// variadic templates (for which we can never come up with enough
/// specializations).

namespace rhine {
/// Test that SourcePrg parses to the given Rhine IR; all transforms are run,
/// and no LLVM operation happens.
template <typename... Ts>
void EXPECT_IR(std::string &SourcePrg, Ts... Matchers) {
  ParseFacade Pf(SourcePrg);
  auto Source = Pf.parseAction(ParseSource::STRING, PostParseAction::IR);
  for (auto ExpectedIR : {Matchers...})
    EXPECT_PRED_FORMAT2(::testing::IsSubstring, ExpectedIR.c_str(),
                        Source.c_str());
}

/// Test the output of Module->dump() for an LLVM IR Module.
template <typename... Ts>
void EXPECT_LL(std::string &SourcePrg, Ts... Matchers) {
  ParseFacade Pf(SourcePrg);
  auto Source = Pf.parseAction(ParseSource::STRING, PostParseAction::LL);
  for (auto ExpectedLL : {Matchers...})
    EXPECT_PRED_FORMAT2(::testing::IsSubstring, ExpectedLL.c_str(),
                        Source.c_str());
}

/// Run the program and expect an the given string on stdout.
template <typename... Ts>
void EXPECT_OUTPUT(std::string &SourcePrg, Ts... Matchers) {
  ParseFacade Pf(SourcePrg);
  auto Handle = Pf.jitAction(ParseSource::STRING, PostParseAction::LL);
  testing::internal::CaptureStdout();
  Handle();
  std::string ActualOut = testing::internal::GetCapturedStdout();
  for (auto ExpectedOut : {Matchers...})
    EXPECT_STREQ(ExpectedOut.c_str(), ActualOut.c_str());
}

/// Test that the program fails to run with the given error message.
template <typename... Ts>
void EXPECT_COMPILE_DEATH(std::string &SourcePrg, Ts... Matchers) {
  ParseFacade Pf(SourcePrg);
  for (auto ExpectedErr : {Matchers...})
    EXPECT_DEATH(Pf.parseAction(ParseSource::STRING, PostParseAction::LL),
                ExpectedErr);
}
}

#endif
