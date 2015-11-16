#include "rhine/Util/TestUtil.h"
#include "gtest/gtest.h"

using namespace rhine;

TEST(If, DanglingIf)
{
  auto SourcePrg =
  CAT_RH(
    def foo do
      if false);
  auto ExpectedErr =
    "string stream:1:20: error: expected 'do' to start block";
  EXPECT_COMPILE_DEATH(SourcePrg, ExpectedErr);
}

TEST(If, SimpleIf)
{
  auto SourcePrg =
  CAT_RH(
    def foo do
      if false do
        print '2';
      else
        print '3';
      end
    end);
  EXPECT_SUCCESSFUL_PARSE(SourcePrg);
}

TEST(If, IfWithoutElseClause)
{
  auto SourcePrg =
  CAT_RH(
    def foo do
      if false do
        print '2';
      end
    end);
  EXPECT_SUCCESSFUL_PARSE(SourcePrg);
}

TEST(If, DISABLED_AssignmentIf)
{
  auto SourcePrg =
  CAT_RH(
    def foo do
      X =
      if false do
        2;
      else
        3;
      end
    end);
  EXPECT_SUCCESSFUL_PARSE(SourcePrg);
}

TEST(If, BasicCodeGen)
{
  auto SourcePrg =
    "def main do\n"
    "  if false do\n"
    "    X = 2\n"
    "  else\n"
    "    Y = 3\n"
    "  end\n"
    "end";
  auto ExpectedLL =
    "define void @main() gc \"rhgc\" {\n"
    "entry:\n"
    "  br i1 false, label %true, label %false\n\n"
    "true:                                             ; preds = %entry\n"
    "  %Alloc = call i8* @std_String_malloc__Int(i64 4)\n"
    "  %0 = bitcast i8* %Alloc to i32*\n"
    "  store i32 2, i32* %0\n"
    "  br label %phi\n\n"
    "false:                                            ; preds = %entry\n"
    "  %Alloc1 = call i8* @std_String_malloc__Int(i64 4)\n"
    "  %1 = bitcast i8* %Alloc1 to i32*\n"
    "  store i32 3, i32* %1\n"
    "  br label %phi\n\n"
    "phi:                                              ; preds = %false, %true\n"
    "  ret void\n"
    "}";
  EXPECT_LL(SourcePrg, ExpectedLL);
}

TEST(If, SideEffectual)
{
  auto SourcePrg =
    CAT_RH(
      def main do
        if false do
        print '2';
        else print '3';
      end
    end);
  auto ExpectedOut = "3";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}

TEST(If, DISABLED_PhiAssignment)
{
  auto SourcePrg =
    CAT_RH(
      def main do
        x =
        if false do
          2;
        else
          3;
        end
      print x;
    end);
  auto ExpectedOut = "3";
  EXPECT_OUTPUT(SourcePrg, ExpectedOut);
}

TEST(If, LifeAfterPhi)
{
  auto SourcePrg =
    "def main do\n"
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
