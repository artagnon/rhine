#include "rhine/Util/TestUtil.h"
#include "gtest/gtest.h"

#include "rhine/Toplevel/ParseFacade.h"
#include "rhine/IR/Module.h"

using namespace rhine;

TEST(Module, CountObjects)
{
  std::string SourcePrg =
    "def main do\n"
    "  print 4;\n"
    "end\n"
    "def foom do"
    "  ret 2;\n"
    "end\n"
    "def baz do\n"
    "  ret print;\n"
    "end";
  ParseFacade Pf(SourcePrg);
  auto Module = Pf.parseToIR(ParseSource::STRING, { });
  ASSERT_EQ(std::distance(Module->begin(), Module->end()), 3);
}

TEST(Module, SetFunctionParent)
{
  std::string SourcePrg =
    "def main do\n"
    "  print 4;\n"
    "end";
  ParseFacade Pf(SourcePrg);
  auto Module = Pf.parseToIR(ParseSource::STRING, { });
  for (auto F : *Module)
    ASSERT_EQ(F->getParent(), Module);
}
