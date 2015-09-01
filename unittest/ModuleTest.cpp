#include "rhine/TestUtil.h"
#include "gtest/gtest.h"

#include "rhine/IR/Module.h"

using namespace rhine;

TEST(Module, CountObjects)
{
  std::string SourcePrg =
    "def main [] {\n"
    "  print 4;\n"
    "}\n"
    "def foom [] 2;\n"
    "def baz[] print;\n";
  auto Pf = ParseFacade(SourcePrg);
  auto Module = Pf.parseToIR(ParseSource::STRING, { });
  ASSERT_EQ(std::distance(Module->begin(), Module->end()), 3);
}

TEST(Module, SetFunctionParent)
{
  std::string SourcePrg =
    "def main [] {\n"
    "  print 4;\n"
    "}";
  auto Pf = ParseFacade(SourcePrg);
  auto Module = Pf.parseToIR(ParseSource::STRING, { });
  for (auto F : *Module)
    ASSERT_EQ(F->getParent(), Module);
}
