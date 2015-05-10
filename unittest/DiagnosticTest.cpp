#include "rhine/Diagnostic.h"
#include "rhine/StdCapture.h"
#include "gtest/gtest.h"

using namespace llvm;
using namespace clang;

TEST(Diagnostic, Basic)
{
  DiagnosticOptions *DiagOpts = new DiagnosticOptions();
  std::string Scratch;
  raw_string_ostream StringStream(Scratch);
  TextDiagnosticPrinter *DiagClient =
    new TextDiagnosticPrinter(StringStream, DiagOpts);
  DiagnosticsEngine Diags(new DiagnosticIDs(),
                          new DiagnosticOptions,
                          DiagClient);
  Diags.Report(diag::err_target_unknown_triple) << "unknown";
  EXPECT_PRED_FORMAT2(::testing::IsSubstring,
                      "error: unknown target triple 'unknown'",
                      Scratch.c_str());
}
