#include "rhine/Util/TestUtil.hpp"
#include "gtest/gtest.h"

#include "rhine/Toplevel/ParseFacade.hpp"
#include "rhine/IR/Module.hpp"

using namespace rhine;

TEST(Module, CountObjects)
{
  auto  SourcePrg =
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
  auto Module = Pf.parseToIR<>(ParseSource::STRING);
  EXPECT_EQ(std::distance(Module->begin(), Module->end()), 3);
}

TEST(Module, SetFunctionParent)
{
  auto  SourcePrg =
    "def main do\n"
    "  print 4;\n"
    "end";
  ParseFacade Pf(SourcePrg);
  auto Module = Pf.parseToIR<>(ParseSource::STRING);
  for (auto F : *Module)
    EXPECT_EQ(F->parent(), Module);
}
