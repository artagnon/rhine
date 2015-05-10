#include "clang/Basic/SourceManager.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/DiagnosticIDs.h"
#include "clang/Basic/DiagnosticOptions.h"
#include "clang/Basic/FileManager.h"
#include "clang/Basic/LangOptions.h"
#include "clang/Basic/TargetInfo.h"
#include "clang/Basic/TargetOptions.h"
#include "clang/Lex/HeaderSearch.h"
#include "clang/Lex/HeaderSearchOptions.h"
#include "clang/Lex/ModuleLoader.h"
#include "clang/Lex/Preprocessor.h"
#include "clang/Lex/PreprocessorOptions.h"
#include "clang/Frontend/TextDiagnosticPrinter.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/Config/llvm-config.h"
#include "gtest/gtest.h"

using namespace llvm;
using namespace clang;

TEST(DiagClang, Basic)
{
  std::string Scratch;
  raw_string_ostream StringStream(Scratch);
  DiagnosticOptions *DiagOpts = new DiagnosticOptions();
  TextDiagnosticPrinter *DiagClient =
    new TextDiagnosticPrinter(StringStream, DiagOpts);
  DiagnosticsEngine Diags(new DiagnosticIDs(),
                          DiagOpts, DiagClient);
  Diags.Report(diag::err_target_unknown_triple) << "unknown";
  EXPECT_PRED_FORMAT2(::testing::IsSubstring,
                      "error: unknown target triple 'unknown'",
                      Scratch.c_str());
}

TEST(DiagClang, SourceManager)
{
  FileSystemOptions FileMgrOpts;
  FileManager FileMgr(FileMgrOpts);

  // Build Diags
  std::string Scratch;
  raw_string_ostream StringStream(Scratch);
  DiagnosticOptions *DiagOpts = new DiagnosticOptions();
  DiagOpts->ShowColors = true;
  TextDiagnosticPrinter *DiagClient =
    new TextDiagnosticPrinter(StringStream, DiagOpts);
  DiagClient->BeginSourceFile(LangOptions(), nullptr);
  DiagnosticsEngine Diags(new DiagnosticIDs(),
                          DiagOpts, DiagClient);

  // Build SourceLocation from SourceManager
  SourceManager SourceMgr(Diags, FileMgr);
  const char *source =
    "#define M(x) [x]\n"
    "M(foo)";
  std::unique_ptr<MemoryBuffer> Buf = MemoryBuffer::getMemBuffer(source);
  FileID mainFileID = SourceMgr.createFileID(std::move(Buf));
  SourceMgr.setMainFileID(mainFileID);
  SourceLocation StartLoc = SourceMgr.translateLineCol(mainFileID, 2, 3);
  SourceLocation EndLoc = SourceMgr.translateLineCol(mainFileID, 2, 5);

  // Report and check
  Diags.Report(StartLoc, diag::err_target_unknown_triple)
    << "unknown" << SourceRange(StartLoc, EndLoc);
  EXPECT_PRED_FORMAT2(::testing::IsSubstring,
                      "M(foo)\n"
                      "  ^~~\n",
                      Scratch.c_str());
}
