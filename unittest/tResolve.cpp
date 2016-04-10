#include "rhine/Util/TestUtil.hpp"
#include "gtest/gtest.h"

#include "rhine/IR/Value.hpp"
#include "rhine/IR/Module.hpp"
#include "rhine/IR/Constant.hpp"
#include "rhine/IR/Instruction.hpp"
#include "rhine/IR/UnresolvedValue.hpp"
#include "rhine/Transform/Resolve.hpp"
#include "rhine/Toplevel/ParseFacade.hpp"

using namespace rhine;

TEST(Resolve, LocalVariable) {
  auto SourcePrg = "def bsym do"
                   "  Sym = 3;\n"
                   "  ret Sym;\n"
                   "end";
  EXPECT_LL(SourcePrg, "bitcast i8* %Alloc to i32*", "store i32 3, i32* %0",
            "load i32, i32* %0");
}

TEST(Resolve, UnresolvedReplacement) {
  auto SourcePrg = "def main do\n"
                   "  Moo = 3;\n"
                   "  Moo + 3;\n"
                   "  Moo + 2;\n"
                   "end";
  ParseFacade Pf(SourcePrg);
  auto Module = Pf.parseToIR<Resolve>(ParseSource::STRING);
  auto MainF = Module->front();
  rhine::Value *Decl = nullptr, *FirstInstance = nullptr,
               *SecondInstance = nullptr;
  for (auto V : *MainF->getEntryBlock()) {
    ASSERT_EQ(dyn_cast<UnresolvedValue>(V), nullptr);
    if (auto D = dyn_cast<MallocInst>(V))
      Decl = D;
    if (auto A = dyn_cast<BinaryArithInst>(V)) {
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

TEST(Resolve, ArgumentSymbolReplacement) {
  auto SourcePrg = "def main(var Int) do\n"
                   "  ret var\n"
                   "end";
  ParseFacade Pf(SourcePrg);
  auto Module = Pf.parseToIR<Resolve>(ParseSource::STRING);
  auto Expected = "def main [var Int] Fn(Int -> UnType) {\n"
                  "ret var Int\n"
                  "}";
  EXPECT_PRED_FORMAT2(::testing::IsSubstring, Expected, Pf.irToPP(Module));
  EXPECT_LL(SourcePrg, "define i32 @main(i32)", "ret i32 %0");
}

TEST(Resolve, CrossFunctionNameDisambiguation) {
  auto SourcePrg =
      "def bar(arithFn Function(Int -> Int -> Int)) do\n"
      "  print $ arithFn 2 4;\n"
      "end\n"
      "def addCandidate(A Int, B Int) do\n"
      "  ret $ A + B;\n"
      "end\n"
      "def subCandidate(A Int, B Int) do\n"
      "  ret $ A - B;\n"
      "end\n"
      "def main() do\n"
      "  if false do bar addCandidate; else bar subCandidate; end\n"
      "end";
  EXPECT_OUTPUT(SourcePrg, "-2");
}

TEST(Resolve, OutOfScope) {
  auto SourcePrg = "def main do\n"
                   "  if true do\n"
                   "     Moo = 2;\n"
                   "  else\n"
                   "     Foo = 4;\n"
                   "  end\n"
                   "  print Moo;\n"
                   "end";
  auto ExpectedErr = "string stream:7:9: error: unbound symbol Moo";
  EXPECT_COMPILE_DEATH(SourcePrg, ExpectedErr);
}

TEST(Resolve, CrossBlockDisambiguation) {
  auto SourcePrg = "def main do\n"
                   "  if true do\n"
                   "     Moo = 2;\n"
                   "  else\n"
                   "     Moo = 4;\n"
                   "  end\n"
                   "end";
  EXPECT_OUTPUT(SourcePrg, "");
}
