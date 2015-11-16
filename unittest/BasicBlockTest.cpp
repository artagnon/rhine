#include "rhine/Util/TestUtil.h"
#include "gtest/gtest.h"

#include "rhine/IR/Module.h"
#include "rhine/IR/Instruction.h"
#include "rhine/Toplevel/ParseFacade.h"

using namespace rhine;

TEST(BasicBlock, SetInstructionParent)
{
  std::string SourcePrg =
    "def main do\n"
    "  2 + 3;\n"
    "  print 4;\n"
    "end";
  ParseFacade Pf(SourcePrg);
  auto Module = Pf.parseToIR(ParseSource::STRING, { });
  for (auto BB : *Module->front())
    for (auto Inst : *BB)
      ASSERT_EQ(cast<rhine::Instruction>(Inst)->getParent(), BB);
}

TEST(BasicBlock, SetNestedInstructionParent)
{
  std::string SourcePrg =
    "def main do\n"
    "  print $ 2 + 4;\n"
    "end";
  ParseFacade Pf(SourcePrg);
  auto Module = Pf.parseToIR(ParseSource::STRING, { });
  for (auto BB : *Module->front())
    for (auto Inst : *BB)
      if (auto OuterInst = dyn_cast<rhine::Instruction>(Inst))
        if (auto NestedInst = dyn_cast<rhine::Instruction>(OuterInst->getOperand(0)))
          ASSERT_EQ(cast<rhine::Instruction>(NestedInst)->getParent(), BB);
}
