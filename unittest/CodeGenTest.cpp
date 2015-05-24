#include "rhine/IR.h"
#include "rhine/Support.h"
#include "gtest/gtest.h"

void EXPECT_PARSE_PP(std::string SourcePrg, std::string ExpectedPP)
{
  std::ostringstream Scratch;
  auto Source = rhine::parseCodeGenString(SourcePrg, Scratch);
  auto ActualErr = Scratch.str();
  ASSERT_STREQ("", ActualErr.c_str());
  EXPECT_PRED_FORMAT2(::testing::IsSubstring, ExpectedPP.c_str(),
                      Source.c_str());
}

TEST(CodeGen, DefunStm)
{
  std::string SourcePrg = "defun foo [] 3 + 2;";
  std::string ExpectedPP =
    "define i32 @foo() {\n"
    "entry:\n"
    "  ret i32 5\n"
    "}\n";
  EXPECT_PARSE_PP(SourcePrg, ExpectedPP);
}

TEST(CodeGen, DefunCompoundStm)
{
  std::string SourcePrg =
    "defun foo []\n"
    "{\n"
    "  3 + 2;\n"
    "  4 + 5;\n"
    "}";
  std::string ExpectedPP =
    "define i32 @foo() {\n"
    "entry:\n"
    "  ret i32 9\n"
    "}\n";
  EXPECT_PARSE_PP(SourcePrg, ExpectedPP);
}

TEST(CodeGen, MultipleDefun)
{
  std::string SourcePrg =
    "defun foo [] 2;\n"
    "defun bar [] 3;\n";
  std::string ExpectedPP =
    "define i32 @foo() {\n"
    "entry:\n"
    "  ret i32 2\n"
    "}\n\n"
    "define i32 @bar() {\n"
    "entry:\n"
    "  ret i32 3\n"
    "}\n";
  EXPECT_PARSE_PP(SourcePrg, ExpectedPP);
}

TEST(CodeGen, FunctionCall)
{
  std::string SourcePrg = "defun foom [] printf \"43\";";
  std::string ExpectedPP =
    "call i32 (i8*, ...) @printf";
  EXPECT_PARSE_PP(SourcePrg, ExpectedPP);
}

TEST(CodeGen, TypeAnnotation)
{
  std::string SourcePrg =
    "defun id [var ~Int] 0;\n";
  std::string ExpectedPP =
    "define i32 @id(i32) {\n"
    "entry:\n"
    "  ret i32 0\n"
    "}\n";
  EXPECT_PARSE_PP(SourcePrg, ExpectedPP);
}

TEST(CodeGen, FunctionArgBinding)
{
  std::string SourcePrg =
    "defun id [var ~Int] var ~Int;\n";
  std::string ExpectedPP =
    "define i32 @id(i32) {\n"
    "entry:\n"
    "  ret i32 %0\n"
    "}\n";
  EXPECT_PARSE_PP(SourcePrg, ExpectedPP);
}

TEST(CodeGen, TypePropagation)
{
  std::string SourcePrg =
    "defun id [var ~Int] var;\n";
  std::string ExpectedPP =
    "define i32 @id(i32) {\n"
    "entry:\n"
    "  ret i32 %0\n"
    "}\n";
  EXPECT_PARSE_PP(SourcePrg, ExpectedPP);
}

TEST(CodeGen, BindPropagation) {
  std::string SourcePrg =
    "defun bsym [] {"
    "sym = 3;\n"
    "sym;\n"
    "}";
  std::string ExpectedPP =
    "define i32 @bsym() {\n"
    "entry:\n"
    "  ret i32 3\n"
    "}\n";
  EXPECT_PARSE_PP(SourcePrg, ExpectedPP);
}

TEST(CodeGen, ExternalsCaching) {
  std::string SourcePrg =
    "defun compside [] {"
    "printf \"foom\";\n"
    "printf \"baz\";\n"
    "}";
  std::string ExpectedPP =
    "  %printf = call i32 (i8*, ...) @printf"
    "(i8* getelementptr inbounds ([5 x i8], [5 x i8]* @0, i32 0, i32 0))\n"
    "  %printf1 = call i32 (i8*, ...) @printf"
    "(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @1, i32 0, i32 0))";
  EXPECT_PARSE_PP(SourcePrg, ExpectedPP);
}

TEST(CodeGen, VoidRepresentation)
{
  std::string SourcePrg =
    "defun id [] var = 3;\n";
  std::string ExpectedPP =
    "define void @id() {\n"
    "entry:\n"
    "  ret void\n"
    "}\n";
  EXPECT_PARSE_PP(SourcePrg, ExpectedPP);
}

TEST(CodeGen, CallInstTyInfer) {
  std::string SourcePrg =
    "defun mallocCall [] {\n"
    "  malloc 8;\n"
    "}";
  std::string ExpectedPP =
    "define i8* @mallocCall() {\n"
    "entry:\n"
    "  %malloc = call i8* @malloc(i64 8)\n"
    "  ret i8* %malloc\n"
    "}\n";
  EXPECT_PARSE_PP(SourcePrg, ExpectedPP);
}

TEST(CodeGen, DISABLED_FunctionPointer) {
  std::string SourcePrg =
    "defun callee [] {\n"
    "  3;\n"
    "}\n"
    "defun caller [] {\n"
    "  callee;\n"
    "}";
  std::string ExpectedPP =
    "define i32 ()* @caller() {\n"
    "entry:\n"
    "  ret i32 ()* @callee\n"
    "}\n";
  EXPECT_PARSE_PP(SourcePrg, ExpectedPP);
}

TEST(CodeGen, DISABLED_ExternalsFunctionPointer) {
  std::string SourcePrg =
    "defun mallocRet [] {\n"
    "  malloc;\n"
    "}";
  std::string ExpectedPP =
    "define i8* (i64)* @mallocRet() {\n"
    "entry:\n"
    "  ret i8* (i64)* @malloc\n"
    "}\n";
  EXPECT_PARSE_PP(SourcePrg, ExpectedPP);
}
