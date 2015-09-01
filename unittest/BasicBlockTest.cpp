#include "rhine/TestUtil.h"
#include "gtest/gtest.h"

#include "rhine/IR/Module.h"
#include "rhine/IR/Instruction.h"

using namespace rhine;

TEST(BasicBlock, SetInstructionParent)
{
  std::string SourcePrg =
    "def main [] {\n"
    "  2 + 3;\n"
    "  print 4;\n"
    "}";
  auto Pf = ParseFacade(SourcePrg);
  auto Module = Pf.parseToIR(ParseSource::STRING, { });
  for (auto BB : *Module->front())
    for (auto Inst : *BB)
      ASSERT_EQ(cast<rhine::Instruction>(Inst)->getParent(), BB);
}
