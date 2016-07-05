#pragma once

#include "rhine/Toplevel/ParseFacade.hpp"
#include "gtest/gtest.h"
#include "gtest/internal/gtest-port.h"

#include <string>

/// All this stuff needs to be in the header because of templates, especially
/// variadic templates (for which we can never come up with enough
/// specializations).

namespace rhine {
/// Call EXPECT_IR with "" as the second argument.
#define EXPECT_SUCCESSFUL_PARSE(SourcePrg) EXPECT_IR(SourcePrg, "")
typedef ::testing::AssertionResult (*AssertionT)(const char *, const char *,
                                                 const char *, const char *);

inline ::testing::AssertionResult
CmpHelperEQ(bool InvertMatch, const char *NeedleExpr, const char *HaystackExpr,
            std::string Needle, std::string Haystack) {
  using namespace ::testing::internal;

  auto OriginalNeedle = Needle;
  auto OriginalHaystack = Haystack;
  auto isWS = [](char Candidate) {
    return Candidate == '\n' || Candidate == ' ';
  };
  Needle.erase(std::remove_if(Needle.begin(), Needle.end(), isWS),
               Needle.end());
  Haystack.erase(std::remove_if(Haystack.begin(), Haystack.end(), isWS),
                 Haystack.end());

  if ((Haystack.find(Needle) == std::string::npos) == InvertMatch) {
    return ::testing::AssertionSuccess();
  }
  return ::testing::AssertionFailure()
         << "Expected (" << NeedleExpr << ") is:" << std::endl
         << "\"" << OriginalNeedle << "\"" << std::endl
         << "Actual (" << HaystackExpr << ") is:" << std::endl
         << "\"" << OriginalHaystack << "\"";
}

/// Strip whitespace and tell if expected is a substring of actual.
template <bool InvertMatch = false>
inline ::testing::AssertionResult
StripWSisSubstring(const char *expected_expression,
                   const char *actual_expression, std::string expected,
                   std::string actual) {
  return CmpHelperEQ(InvertMatch, expected_expression, actual_expression,
                     expected, actual);
}

/// Test that SourcePrg parses to the given Rhine IR; all transforms are run,
/// and no LLVM operation happens.
template <typename... Ts>
void EXPECT_IR(const char *SourcePrg, Ts... Matchers) {
  ParseFacade Pf(SourcePrg);
  auto Source = Pf.parseAction(ParseSource::STRING, PostParseAction::IRString);
  for (auto ExpectedIR : {Matchers...})
    EXPECT_PRED_FORMAT2(StripWSisSubstring<>, std::string(ExpectedIR), Source);
}

/// Test the output of Module->dump() for an LLVM IR Module.
template <typename... Ts>
void EXPECT_LL(const char *SourcePrg, Ts... Matchers) {
  ParseFacade Pf(SourcePrg);
  auto Source = Pf.parseAction(ParseSource::STRING, PostParseAction::LLString);
  for (auto ExpectedLL : {Matchers...})
    EXPECT_PRED_FORMAT2(StripWSisSubstring<>, std::string(ExpectedLL), Source);
}

template <bool, typename... Ts>
void EXPECT_LL(const char *SourcePrg, Ts... Matchers) {
  ParseFacade Pf(SourcePrg);
  auto Source = Pf.parseAction(ParseSource::STRING, PostParseAction::LLString);
  for (auto ExpectedLL : {Matchers...})
    EXPECT_PRED_FORMAT2(StripWSisSubstring<true>, std::string(ExpectedLL),
                        Source);
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
