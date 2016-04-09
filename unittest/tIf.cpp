#include "rhine/IR/Instruction.hpp"
#include "rhine/IR/Module.hpp"
#include "rhine/Util/TestUtil.hpp"
#include "gtest/gtest.h"

using namespace rhine;

/// Stress operation of if statements.

TEST(If, DanglingIf) {
  auto SourcePrg = "def foo do\n"
                   "  if false";
  auto ExpectedErr = "string stream:2:11: error: expected 'do' to start block";
  EXPECT_COMPILE_DEATH(SourcePrg, ExpectedErr);
}

TEST(If, SimpleIf) {
  auto SourcePrg = "def foo do\n"
                   "  if false\n"
                   "    do print '2'\n"
                   "  else\n"
                   "    print '3'\n"
                   "  end\n"
                   "end";
  EXPECT_SUCCESSFUL_PARSE(SourcePrg);
}

TEST(If, IfWithoutElseClause) {
  auto SourcePrg = "def foo do\n"
                   "  if false do\n"
                   "    print '2'\n"
                   "  end\n"
                   "end";
  EXPECT_SUCCESSFUL_PARSE(SourcePrg);
}

TEST(If, BasicCodeGen) {
  auto SourcePrg = "def main do\n"
                   "  if false do\n"
                   "    X = 2\n"
                   "  else\n"
                   "    Y = 3\n"
                   "  end\n"
                   "end";
  EXPECT_LL(SourcePrg, "br i1 false, label %true, label %false", "true:",
            "false:", "merge:", "call i8* @std_String_malloc__Int(i64 4)",
            "store i32 2, i32* %0", "store i32 3, i32* %1");
}

TEST(If, SideEffectual) {
  auto SourcePrg = "def main do\n"
                   "  if false do\n"
                   "    print '2'\n"
                   "  else\n"
                   "    print '3'\n"
                   "  end\n"
                   "end";
  auto ExpectedOut = "3";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}

TEST(If, LifeAfterPhi) {
  auto SourcePrg = "def main do\n"
                   "  if true do\n"
                   "     Moo = 2;\n"
                   "  else\n"
                   "     Foo = 4;\n"
                   "  end\n"
                   "  print 3;\n"
                   "end";
  auto ExpectedOut = "3";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}

TEST(If, Nested) {
  auto SourcePrg = "def main do\n"
                   "  if true do\n"
                   "    if true do\n"
                   "      print 2\n"
                   "    else\n"
                   "      print 3\n"
                   "    end\n"
                   "  else\n"
                   "    print 4\n"
                   "  end\n"
                   "  print 5\n"
                   "end";
  ParseFacade Pf(SourcePrg);
  auto Mod = Pf.parseToIR(ParseSource::STRING);
  auto EntryBlock = Mod->front()->getEntryBlock();
  auto &InstList = EntryBlock->getInstList();
  EXPECT_EQ(InstList.size(), 1u);
  auto IfStmt = cast<IfInst>(InstList[0]);
  auto &TrueInstList = IfStmt->getTrueBB()->getInstList();
  EXPECT_EQ(TrueInstList.size(), 1u);
  EXPECT_EQ(IfStmt->getFalseBB()->getInstList().size(), 1u);
  auto NestedIfStmt = cast<IfInst>(TrueInstList[0]);
  EXPECT_EQ(NestedIfStmt->getTrueBB()->getInstList().size(), 1u);
  EXPECT_EQ(NestedIfStmt->getFalseBB()->getInstList().size(), 1u);
  EXPECT_OUTPUT(SourcePrg, "25");
}

TEST(If, MergeInBranch) {
  auto SourcePrg = "def main do\n"
                   "  if true do\n"
                   "    if true do\n"
                   "      print 2\n"
                   "    else\n"
                   "      print 3\n"
                   "    end\n"
                   "    print 4\n"
                   "  end\n"
                   "  print 5\n"
                   "end";
  EXPECT_OUTPUT(SourcePrg, "245");
}

TEST(If, PhiAssignment) {
  auto SourcePrg = "def main do\n"
                   "  x =\n"
                   "    if false do 2\n"
                   "    else 3\n"
                   "    end\n"
                   "  print x\n"
                   "end";
  auto ExpectedOut = "3";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}

TEST(If, PhiAssignment_FunctionPointer) {
  auto SourcePrg = "def addCandidate(A ~Int, B ~Int) do\n"
                   "  ret $ A + B\n"
                   "end\n"
                   "def subCandidate(C ~Int, D ~Int) do\n"
                   "  ret $ C - D\n"
                   "end\n"
                   "def main do\n"
                   "  X = if false do\n"
                   "    addCandidate\n"
                   "  else\n"
                   "    subCandidate\n"
                   "  end\n"
                   "  print $ X 3 4\n"
                   "end";
  auto ExpectedOut = "-1";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}
