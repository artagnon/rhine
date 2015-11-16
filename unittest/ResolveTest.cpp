#include "rhine/Util/TestUtil.h"
#include "gtest/gtest.h"

#include "rhine/IR/Value.h"
#include "rhine/IR/Module.h"
#include "rhine/IR/Constant.h"
#include "rhine/IR/Instruction.h"
#include "rhine/IR/UnresolvedValue.h"
#include "rhine/Transform/Resolve.h"
#include "rhine/Toplevel/ParseFacade.h"

using namespace rhine;

TEST(Resolve, UnresolvedReplacement)
{
  std::string SourcePrg =
    "def main do\n"
    "  Moo = 3;\n"
    "  Moo + 3;\n"
    "  Moo + 2;\n"
    "end";
  ParseFacade Pf(SourcePrg);
  Resolve ResolveL;
  auto Module = Pf.parseToIR(ParseSource::STRING, { &ResolveL });
  auto MainF = Module->front();
  rhine::Value *Decl = nullptr,
    *FirstInstance = nullptr,
    *SecondInstance = nullptr;
  for (auto V : *MainF->getEntryBlock()) {
    ASSERT_EQ(dyn_cast<UnresolvedValue>(V), nullptr);
    if (auto D = dyn_cast<MallocInst>(V))
      Decl = D;
    if (auto A = dyn_cast<AddInst>(V)) {
      auto Op0 = A->getOperand(0);
      auto Op1 = A->getOperand(1);
      if (auto U = dyn_cast<rhine::LoadInst>(Op0))
        if (auto C = dyn_cast<rhine::ConstantInt>(Op1)) {
          if (C->getVal() == 3)
            FirstInstance = U;
          else
            SecondInstance = U;
        }
    }
  }
  ASSERT_NE(Decl, nullptr);
  ASSERT_NE(FirstInstance, nullptr);
  ASSERT_NE(SecondInstance, nullptr);
  ASSERT_NE(FirstInstance, SecondInstance);
}

TEST(Resolve, ArgumentSymbolReplacement)
{
  std::string SourcePrg =
    "def main(var ~Int) do\n"
    "  ret var\n"
    "end";
  ParseFacade Pf(SourcePrg);
  Resolve ResolveL;
  auto Module = Pf.parseToIR(ParseSource::STRING, { &ResolveL });
  auto Expected =
    "def main [var ~Int] ~Fn(Int -> UnType) {\n"
    "ret var ~Int\n"
    "}";
  EXPECT_PRED_FORMAT2(::testing::IsSubstring, Expected, Pf.irToPP(Module));
}

TEST(Resolve, CrossFunctionNameDisambiguation)
{
  std::string SourcePrg =
    "def bar(arithFn ~Function(Int -> Int -> Int)) do\n"
    "  print $ arithFn 2 4;\n"
    "end\n"
    "def addCandidate(A ~Int B ~Int) do\n"
    "  ret $ A + B;\n"
    "end\n"
    "def subCandidate(A ~Int B ~Int) do\n"
    "  ret $ A - B;\n"
    "end\n"
    "def main() do\n"
    "  if false do bar addCandidate; else bar subCandidate; end\n"
    "end";
  std::string ExpectedOut = "-2";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}

TEST(Resolve, OutOfScope)
{
  std::string SourcePrg =
    "def main do\n"
    "  if true do\n"
    "     Moo = 2;\n"
    "  else\n"
    "     Foo = 4;\n"
    "  end\n"
    "  print Moo;\n"
    "end";
  std::string ExpectedErr = "string stream:7:9: error: unbound symbol Moo";
  EXPECT_COMPILE_DEATH(SourcePrg, ExpectedErr);
}

TEST(Resolve, DISABLED_CrossBlockDisambiguation)
{
  std::string SourcePrg =
    "def main do\n"
    "  if true do\n"
    "     Moo = 2;\n"
    "  else\n"
    "     Moo = 4;\n"
    "  end\n"
    "end";
  std::string ExpectedOut = "";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}
