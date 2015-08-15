#include "rhine/TestUtil.h"
#include "gtest/gtest.h"

#include "rhine/IR/Value.h"
#include "rhine/IR/Instruction.h"
#include "rhine/IR/UnresolvedValue.h"
#include "rhine/Transform/ResolveLocals.h"

using namespace rhine;

TEST(ResolveLocals, Basic)
{
  std::string SourcePrg =
    "def main [] {\n"
    "  Moo = 3;\n"
    "  Moo + 3;\n"
    "  Moo + 2;\n"
    "}";
  auto Pf = ParseFacade(SourcePrg);
  ResolveLocals ResolveL;
  auto Module = Pf.parseToIR(ParseSource::STRING, {&ResolveL});
  auto MainF = Module->front();
  rhine::Value *Decl = nullptr,
    *FirstInstance = nullptr,
    *SecondInstance = nullptr;
  for (auto V : *MainF) {
    ASSERT_EQ(dyn_cast<UnresolvedValue>(V), nullptr);
    if (auto D = dyn_cast<MallocInst>(V))
      Decl = D;
    if (auto A = dyn_cast<AddInst>(V))
      if (auto U = dyn_cast<rhine::LoadInst>(A->getOperand(0)))
        if (auto C = dyn_cast<rhine::ConstantInt>(A->getOperand(1))) {
          if (C->getVal() == 3)
            FirstInstance = U;
          else
            SecondInstance = U;
        }
  }
  ASSERT_NE(Decl, nullptr);
  ASSERT_NE(FirstInstance, nullptr);
  ASSERT_NE(SecondInstance, nullptr);
  ASSERT_NE(FirstInstance, SecondInstance);
}
