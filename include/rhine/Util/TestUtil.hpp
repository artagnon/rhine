//-*- C++ -*-

#ifndef RHINE_TESTUTIL_H
#define RHINE_TESTUTIL_H

#include "rhine/Toplevel/ParseFacade.hpp"
#include "gtest/gtest.h"
#include "gtest/internal/gtest-port.h"

#include <string>

/// All this stuff needs to be in the header because of templates, especially
/// variadic templates (for which we can never come up with enough
/// specializations).

/// Call EXPECT_IR with "" as the second argument.
#define EXPECT_SUCCESSFUL_PARSE(SourcePrg) EXPECT_IR(SourcePrg, "")
typedef ::testing::AssertionResult (*AssertionT)(const char *, const char *,
                                                 const char *, const char *);

namespace rhine {
/// Test that SourcePrg parses to the given Rhine IR; all transforms are run,
/// and no LLVM operation happens.
template <typename... Ts>
void EXPECT_IR(const char *SourcePrg, Ts... Matchers) {
  ParseFacade Pf(SourcePrg);
  auto Source = Pf.parseAction(ParseSource::STRING, PostParseAction::IRString);
  for (auto ExpectedIR : {Matchers...})
    EXPECT_PRED_FORMAT2(::testing::IsSubstring, ExpectedIR, Source.c_str());
}

/// Test the output of Module->dump() for an LLVM IR Module.
template <typename... Ts>
void EXPECT_LL(AssertionT Assertion, const char *SourcePrg, Ts... Matchers) {
  ParseFacade Pf(SourcePrg);
  auto Source = Pf.parseAction(ParseSource::STRING, PostParseAction::LLString);
  for (auto ExpectedLL : {Matchers...})
    EXPECT_PRED_FORMAT2(Assertion, ExpectedLL, Source.c_str());
}

template <typename... Ts>
void EXPECT_LL(const char *SourcePrg, Ts... Matchers) {
  EXPECT_LL(::testing::IsSubstring, SourcePrg, Matchers...);
}

/// Run the program and expect an the given string on stdout.
template <typename... Ts>
void EXPECT_OUTPUT(const char *SourcePrg, Ts... Matchers) {
  ParseFacade Pf(SourcePrg);
  auto Handle = Pf.jitAction(ParseSource::STRING, PostParseAction::LLString);
  testing::internal::CaptureStdout();
  Handle();
  std::string ActualOut = testing::internal::GetCapturedStdout();
  for (auto ExpectedOut : {Matchers...})
    EXPECT_STREQ(ExpectedOut, ActualOut.c_str());
}

/// Test that the program fails to run with the given error message.
template <typename... Ts>
void EXPECT_COMPILE_DEATH(const char *SourcePrg, Ts... Matchers) {
  ParseFacade Pf(SourcePrg);
  for (auto ExpectedErr : {Matchers...})
    EXPECT_DEATH(Pf.parseAction(ParseSource::STRING, PostParseAction::LLString),
                 ExpectedErr);
}
}

#endif
