#include "rhine/Util/TestUtil.h"
#include "gtest/gtest.h"

using namespace rhine;

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
  auto SourcePrg = CAT_RH(def foo do if false do print '2'; end end);
  EXPECT_SUCCESSFUL_PARSE(SourcePrg);
}

TEST(If, DISABLED_AssignmentIf) {
  auto SourcePrg = CAT_RH(def foo do X = if false do 2; else 3; end end);
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
  auto SourcePrg =
      CAT_RH(def main do if false do print '2'; else print '3'; end end);
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
                   "  print 2;\n"
                   "end";
  auto ExpectedOut = "2";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}

TEST(If, DISABLED_PhiAssignment) {
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
