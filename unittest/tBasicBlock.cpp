#include "rhine/Util/TestUtil.hpp"
#include "gtest/gtest.h"

#include "rhine/IR/Module.hpp"
#include "rhine/IR/Instruction.hpp"
#include "rhine/Toplevel/ParseFacade.hpp"

using namespace rhine;

TEST(BasicBlock, SetInstructionParent)
{
  auto SourcePrg =
    "def main do\n"
    "  2 + 3;\n"
    "  print 4;\n"
    "end";
  ParseFacade Pf(SourcePrg);
  auto Module = Pf.parseToIR<>(ParseSource::STRING);
  for (auto BB : *Module->front())
    for (auto Inst : *BB)
      ASSERT_EQ(cast<rhine::Instruction>(Inst)->parent(), BB);
}

TEST(BasicBlock, SetNestedInstructionParent)
{
  auto SourcePrg =
    "def main do\n"
    "  print $ 2 + 4;\n"
    "end";
  ParseFacade Pf(SourcePrg);
  auto Module = Pf.parseToIR<>(ParseSource::STRING);
  for (auto BB : *Module->front())
    for (auto Inst : *BB)
      if (auto OuterInst = dyn_cast<rhine::Instruction>(Inst))
        if (auto NestedInst = dyn_cast<rhine::Instruction>(OuterInst->getOperand(0)))
          ASSERT_EQ(cast<rhine::Instruction>(NestedInst)->parent(), BB);
}

TEST(BasicBlock, NonInstruction)
{
  auto SourcePrg =
    "def main do\n"
    "  2;\n"
    "end";
  EXPECT_LL(SourcePrg, "ret void");
}

TEST(BasicBlock, MoreNonInstructions)
{
  auto SourcePrg =
    "def main do\n"
    "  2\n"
    "  3\n"
    "end";
  EXPECT_COMPILE_DEATH(SourcePrg, "unexpected block terminator");
}
