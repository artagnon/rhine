#include "rhine/IR.h"
#include "rhine/Support.h"
#include "gtest/gtest.h"

TEST(Diagnostic, BareDefun)
{
  std::string SourcePrg = "defun foo []";
  std::string ExpectedErr = "string stream:1:13:.*error:.*syntax error";
  EXPECT_DEATH(rhine::parseCodeGenString(SourcePrg, std::cerr), ExpectedErr);
}

TEST(Diagnostic, UnboundVariable)
{
  std::string SourcePrg = "defun foo [] var ~Int;";
  std::string ExpectedErr = "string stream:1:14:.*error:.*unbound variable var";
  EXPECT_DEATH(rhine::parseCodeGenString(SourcePrg, std::cerr), ExpectedErr);
}
