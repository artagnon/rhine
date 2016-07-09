#include "rhine/Util/TestUtil.hpp"
#include "gtest/gtest.h"

#include "rhine/IR/Instruction.hpp"
#include "rhine/IR/Module.hpp"
#include "rhine/Toplevel/ParseFacade.hpp"
#include "rhine/Transform/LambdaLifting.hpp"
#include "rhine/Transform/Scope2Block.hpp"

using namespace rhine;

TEST(Scope2Block, NumberOfBBs) {
  auto SourcePrg = "def main() do\n"
                   "  if false do X = 3; else Y = 4; end\n"
                   "end";
  ParseFacade Pf(SourcePrg);
  auto Mod = Pf.parseToIR<Scope2Block>(ParseSource::STRING);
  auto MainF = Mod->front();
  auto NumberOfBBs = std::distance(MainF->begin(), MainF->end());
  ASSERT_EQ(NumberOfBBs, 4);
}

TEST(Scope2Block, PredSucc) {
  auto SourcePrg = "def main() do\n"
                   "  if false do X = 3; else Y = 4; end\n"
                   "end";
  ParseFacade Pf(SourcePrg);
  auto Mod = Pf.parseToIR<Scope2Block>(ParseSource::STRING);
  auto MainF = Mod->front();
  std::vector<unsigned> NumPreds = {0, 1, 1, 2}, NumSuccs = {2, 1, 1, 0};
  auto NumPredsIt = NumPreds.begin();
  auto NumSuccsIt = NumSuccs.begin();
  for (auto BB : *MainF) {
    ASSERT_EQ(BB->pred_size(), *NumPredsIt++);
    ASSERT_EQ(BB->succ_size(), *NumSuccsIt++);
  }
}

TEST(Scope2Block, SetParent) {
  auto SourcePrg = "def main() do\n"
                   "  ret 4;\n"
                   "end";
  ParseFacade Pf(SourcePrg);
  auto Mod = Pf.parseToIR<Scope2Block>(ParseSource::STRING);
  auto MainF = Mod->front();
  auto EntryBlock = MainF->getEntryBlock();
  ASSERT_EQ(EntryBlock->parent(), MainF);
}

TEST(Scope2Block, SetIfParent) {
  auto SourcePrg = "def main() do\n"
                   "  if false do X = 3; else Y = 4; end\n"
                   "end";
  ParseFacade Pf(SourcePrg);
  auto Mod = Pf.parseToIR<Scope2Block>(ParseSource::STRING);
  auto MainF = Mod->front();
  for (auto BB : *MainF) {
    ASSERT_EQ(BB->parent(), MainF);
  }
}

TEST(Scope2Block, Lambda) {
  auto SourcePrg = "def foo() do\n"
                   "  Bfunc = fn x Int -> ret x; end\n"
                   "end";
  ParseFacade Pf(SourcePrg);
  auto Mod = Pf.parseToIR<Scope2Block>(ParseSource::STRING);
  for (auto F : *Mod)
    for (auto BB : *F)
      ASSERT_EQ(BB->parent(), F);
}

TEST(Scope2Block, LambdaInsideIf) {
  auto SourcePrg = "def foo() do\n"
                   "  if true do\n"
                   "    Bfunc = fn x Int -> ret x; end\n"
                   "  end\n"
                   "end";
  ParseFacade Pf(SourcePrg);
  auto Mod = Pf.parseToIR<Scope2Block>(ParseSource::STRING);
  for (auto F : *Mod)
    for (auto BB : *F)
      ASSERT_EQ(BB->parent(), F);
}

TEST(Scope2Block, NestedIf) {
  auto SourcePrg = "def main() do\n"
                   "  if true do \n"
                   "    if true do\n"
                   "      X = 3\n"
                   "    else\n"
                   "      X = 2\n"
                   "    end\n"
                   "    Z = 10\n"
                   "  else\n"
                   "    Y = 4\n"
                   "  end\n"
                   "end";
  ParseFacade Pf(SourcePrg);
  auto Mod = Pf.parseToIR<Scope2Block>(ParseSource::STRING);
  auto MainF = Mod->front();
  auto NumberOfBBs = std::distance(MainF->begin(), MainF->end());
  ASSERT_EQ(NumberOfBBs, 7);
  std::vector<unsigned> NumPreds = {0, 1, 1, 1, 2, 1, 2},
                        NumSuccs = {2, 2, 1, 1, 1, 1, 0};
  auto NumPredsIt = NumPreds.begin();
  auto NumSuccsIt = NumSuccs.begin();
  for (auto BB : *MainF) {
    SCOPED_TRACE("Array index: " +
                 std::to_string(std::distance(NumPreds.begin(), NumPredsIt)));
    ASSERT_EQ(BB->pred_size(), *NumPredsIt++);
    ASSERT_EQ(BB->succ_size(), *NumSuccsIt++);
  }
}

TEST(Scope2Block, MergeInBranch) {
  auto SourcePrg = "def main do\n"
                   "  if true do\n"
                   "    if true do\n"
                   "      print 2\n"
                   "    end\n"
                   "    print 4\n"
                   "  end\n"
                   "end";
  ParseFacade Pf(SourcePrg);
  auto Mod = Pf.parseToIR<Scope2Block>(ParseSource::STRING);
  auto MainF = Mod->front();
  auto EntryBlock = MainF->getEntryBlock();
  auto InstList = EntryBlock->getInstList();
  auto IfStmt = cast<IfInst>(InstList.front());
  auto TrueIt = std::find(MainF->begin(), MainF->end(), IfStmt->getTrueBB());
  auto FalseIt = std::find(MainF->begin(), MainF->end(), IfStmt->getFalseBB());
  EXPECT_EQ(std::distance(TrueIt, FalseIt), 4);
}
