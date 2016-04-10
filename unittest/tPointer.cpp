#include "rhine/Util/TestUtil.hpp"
#include "gtest/gtest.h"

#include "rhine/IR/Instruction.hpp"
#include "rhine/IR/Module.hpp"
#include "rhine/Toplevel/ParseFacade.hpp"

using namespace rhine;

TEST(Pointer, AssignToPointer) {
  auto SourcePrg = "def foo do\n"
                   "A = 3\n"
                   "B = A\n"
                   "end";
  EXPECT_LL(SourcePrg, "%ALoad = load i32, i32*",
            "%Alloc = call i8* @std_String_malloc__Int",
            "%Alloc1 = call i8* @std_String_malloc__Int");
}

TEST(Pointer, Types) {
  auto SourcePrg = "def foo do\n"
                   "A = 3\n"
                   "B = A\n"
                   "end";
  ParseFacade Pf(SourcePrg);
  auto Module = Pf.parseToIR(ParseSource::STRING);
  for (auto BB : *Module->find("foo"))
    for (auto Inst : *BB)
      EXPECT_NE(dyn_cast<MallocInst>(Inst), nullptr);
}
