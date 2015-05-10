#include "rhine/Diagnostic.h"
#include "rhine/StdCapture.h"
#include "gtest/gtest.h"

using namespace llvm;
using namespace clang;

TEST(Diagnostic, Basic)
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

TEST(Diagnostic, SourceManager)
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
  SourceLocation FaultyLocation = SourceMgr.translateLineCol(mainFileID, 2, 1);

  // Report and check
  Diags.Report(FaultyLocation, diag::err_target_unknown_triple) << "unknown";
  EXPECT_PRED_FORMAT2(::testing::IsSubstring,
                      "M(foo)\n"
                      "^\n",
                      Scratch.c_str());
}
