#include "rhine/Util/TestUtil.hpp"
#include "gtest/gtest.h"

using namespace rhine;

TEST(Parse, BareDefun) {
  auto SourcePrg = "def foo";
  auto ExpectedErr = "string stream:1:8: error: expected 'do' to start block";
  EXPECT_COMPILE_DEATH(SourcePrg, ExpectedErr);
}

TEST(Parse, BareDanglingDo) {
  auto SourcePrg = "def foo do";
  auto ExpectedErr = "string stream:1:11: error: expected 'end' to end block";
  EXPECT_COMPILE_DEATH(SourcePrg, ExpectedErr);
}

TEST(Parse, DanglingDoWithStatement) {
  auto SourcePrg = "def foo do 3;";
  auto ExpectedErr = "string stream:1:14: error: expected 'end' to end block";
  EXPECT_COMPILE_DEATH(SourcePrg, ExpectedErr);
}

TEST(Parse, EOFExpected) {
  auto SourcePrg = "def foo do 3; end;";
  auto ExpectedErr = "string stream:1:18: error: expected end of file";
  EXPECT_COMPILE_DEATH(SourcePrg, ExpectedErr);
}

TEST(Parse, DanglingFunctionType) {
  auto SourcePrg = "def foo(A ~Function(Void ->";
  auto ExpectedErr =
      "string stream:1:28: error: dangling function type specifier";
  EXPECT_COMPILE_DEATH(SourcePrg, ExpectedErr);
}

TEST(Parse, MalformedFunctionType) {
  auto SourcePrg = "def foo(A ~Function(Void ->)";
  auto ExpectedErr =
      "string stream:1:28: error: dangling function type specifier";
  EXPECT_COMPILE_DEATH(SourcePrg, ExpectedErr);
}

TEST(Parse, FunctionTypeMissingOpenParen) {
  auto SourcePrg = "def foo(A ~Function) 3;";
  auto ExpectedErr = "string stream:1:20: error: in function type of form "
                     "'Function\\(...\\)', '\\(' is missing";
  EXPECT_COMPILE_DEATH(SourcePrg, ExpectedErr);
}

TEST(Parse, FunctionTypeMissingCloseParen) {
  auto SourcePrg = "def foo(A ~Function(Void -> Void";
  auto ExpectedErr = "string stream:1:33: error: in function type of form "
                     "'Function\\(...\\)', '\\)' is missing";
  EXPECT_COMPILE_DEATH(SourcePrg, ExpectedErr);
}

TEST(Parse, CommaArgList) {
  auto SourcePrg = "def foo(A ~Int, B ~Int) do\n"
                   "end";
  EXPECT_SUCCESSFUL_PARSE(SourcePrg);
}

TEST(Parse, CommaLessArgList) {
  auto SourcePrg = "def foo(A ~Int B ~Int) do\n"
                   "end";
  auto ExpectedErr = "string stream:1:16: error: expected ')' to end function "
                     "argument list, or ',' to separate arguments";
  EXPECT_COMPILE_DEATH(SourcePrg, ExpectedErr);
}

TEST(Parse, OnelinerRet) {
  auto SourcePrg = "def foo() do ret 3 end";
  EXPECT_SUCCESSFUL_PARSE(SourcePrg);
}

TEST(Parse, MultilineDoBlock) {
  auto SourcePrg = "def foo() do\n"
                   "  ret 3\n"
                   "end";
  EXPECT_SUCCESSFUL_PARSE(SourcePrg);
}

TEST(Parse, ExtraNewlines) {
  auto SourcePrg = "def foo() do\n\n\n\n"
                   "  ret 3\n\n"
                   "end\n\n";
  EXPECT_SUCCESSFUL_PARSE(SourcePrg);
}

TEST(Parse, AssignmentArithOp) {
  auto SourcePrg = "def foo() do\n"
                   "  Foo = 2 + 3\n"
                   "  Bar = 5 - 19\n"
                   "end";
  EXPECT_SUCCESSFUL_PARSE(SourcePrg);
}

TEST(Parse, MixedSemicolonNewline) {
  auto SourcePrg = "def foo() do\n"
                   "  Foo = 2; ret 3\n"
                   "end";
  EXPECT_SUCCESSFUL_PARSE(SourcePrg);
}

TEST(Parse, MissingStatementSeparator) {
  auto SourcePrg = "def foo() do\n"
                   "  Foo = 2 ret 3\n"
                   "end";
  auto ExpectedErr =
      "string stream:2:11: error: expecting ';' or newline to terminate bind";
  EXPECT_COMPILE_DEATH(SourcePrg, ExpectedErr);
}

TEST(Parse, CallInst_BackToBack) {
  auto SourcePrg = "def main do\n"
                   "  print 2\n"
                   "  print 2\n"
                   "end";
  EXPECT_SUCCESSFUL_PARSE(SourcePrg);
}

TEST(Parse, CallInst_NoArgument) {
  auto SourcePrg = "def main do\n"
                   "  print\n"
                   "end";
  EXPECT_SUCCESSFUL_PARSE(SourcePrg);
}
