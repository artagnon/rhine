#include "rhine/Util/TestUtil.hpp"
#include "gtest/gtest.h"

using namespace rhine;

/// Stress the type inference engine.

TEST(TyInfer, TypeAnnotation) {
  auto SourcePrg = "def id(var Int) do\n"
                   "  ret 0;\n"
                   "end";
  EXPECT_LL(SourcePrg, "define i32 @id(i32)", "ret i32 0");
}

TEST(TyInfer, CallInst) {
  auto SourcePrg = "def mallocCall do\n"
                   "  ret $ malloc 8;\n"
                   "end";
  EXPECT_LL(SourcePrg, "define i8* @mallocCall()",
            "%malloc = call i8* @std_String_malloc__Int(i64 8)",
            "ret i8* %malloc");
}

TEST(TyInfer, PropagationFromArgument) {
  auto SourcePrg = "def id(var Int) do\n"
                   "  ret var;\n"
                   "end";
  EXPECT_LL(SourcePrg, "define i32 @id(i32)", "ret i32 %0");
}

TEST(TyInfer, DISABLED_InferArgumentFromInstruction) {
  auto SourcePrg = "def pid(var) do\n"
                   "  ret $ var + 2;\n"
                   "end";
  EXPECT_LL(SourcePrg, "define i32 @id(i32)", "ret i32");
}

TEST(TyInfer, InferCallFromFunction) {
  auto SourcePrg = "def cid(var Int) do\n"
                   "  ret $ var + 2;\n"
                   "end\n"
                   "def main do\n"
                   "  ret $ cid 2\n"
                   "end";
  EXPECT_LL(SourcePrg, "define i32 @cid(i32)", "define i32 @main",
            "ret i32 %cid1");
}
