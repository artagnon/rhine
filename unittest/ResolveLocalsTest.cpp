#include "rhine/TestUtil.h"
#include "gtest/gtest.h"

#include "rhine/IR/Value.h"
#include "rhine/IR/Instruction.h"
#include "rhine/IR/UnresolvedValue.h"
#include "rhine/Transform/ResolveLocals.h"

using namespace rhine;

TEST(ResolveLocals, DISABLED_Basic)
{
  std::string SourcePrg =
    "def main [] {\n"
    "  moo = 3;\n"
    "  moo;\n"
    "  moo + 2\n;"
    "}";
  auto Pf = ParseFacade(SourcePrg);
  ResolveLocals ResolveL;
  auto Module = Pf.parseToIR(ParseSource::STRING, {&ResolveL});
  auto MainF = Module->front();
  rhine::Value *Decl = nullptr,
    *FirstInstance = nullptr,
    *SecondInstance = nullptr;
  for (auto V : *MainF) {
    if (auto D = dyn_cast<MallocInst>(V))
      Decl = D;
    if (auto U = dyn_cast<UnresolvedValue>(V))
      FirstInstance = U;
    if (auto A = dyn_cast<AddInst>(V))
      if (auto U = dyn_cast<UnresolvedValue>(A->getOperand(0)))
        SecondInstance = U;
  }
  ASSERT_NE(Decl, nullptr);
  ASSERT_NE(FirstInstance, nullptr);
  ASSERT_NE(SecondInstance, nullptr);
  ASSERT_NE(FirstInstance, SecondInstance);
}
