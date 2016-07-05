#include "rhine/Util/TestUtil.hpp"
#include "gtest/gtest.h"

#include "rhine/IR/Instruction.hpp"
#include "rhine/IR/Module.hpp"
#include "rhine/Toplevel/OptionParser.hpp"
#include "rhine/Toplevel/ParseFacade.hpp"

using namespace rhine;

enum OptionIndex { UNKNOWN, DEBUG, STDIN, HELP };

const option::Descriptor Usage[] = {
    {UNKNOWN, 0, "", "", option::Arg::None,
     "USAGE: Rhine [options] <filename>\n\nOptions:"},
    {DEBUG, 0, "", "debug", option::Arg::None,
     " --debug  \tDebug lexer and parser"},
    {STDIN, 0, "", "stdin", option::Arg::None,
     " --stdin  \tRead input from stdin"},
    {HELP, 0, "", "help", option::Arg::None, " --help  \tPrint usage and exit"},
    {0, 0, 0, 0, 0, 0}};

TEST(CLI, Stdin) {
  auto argc = 2;
  const char *argv[] = {"--debug", "--stdin"};
  option::Stats Stats(Usage, argc, argv);
  auto Options =
      std::unique_ptr<option::Option[]>(new option::Option[Stats.options_max]);
  auto Buffer =
      std::unique_ptr<option::Option[]>(new option::Option[Stats.buffer_max]);
  option::Parser Parse(Usage, argc, argv, Options.get(), Buffer.get());

  ASSERT_FALSE(Parse.error());
  ASSERT_TRUE(Options[STDIN]);
  ASSERT_FALSE(Options[UNKNOWN]);
  ASSERT_TRUE(Options[DEBUG]);

  std::string InFileOrStream = "def main do println 2173; ret 0; end";

  ParseFacade Pf(InFileOrStream, std::cerr, true);
  testing::internal::CaptureStderr();
  auto FHandle = Pf.jitAction(ParseSource::STRING, PostParseAction::LLEmit);
  std::string ActualErr = testing::internal::GetCapturedStderr();
  testing::internal::CaptureStdout();
  FHandle();
  std::string ActualOut = testing::internal::GetCapturedStdout();
  std::string ExpectedErr =
      R"ll(
        define i32 @main() {
        entry:
          call void (i8*, ...) @std_Void_println__String(i8* getelementptr
            inbounds ([5 x i8], [5 x i8]* @0, i32 0, i32 0))
          ret i32 0
        }
      )ll";
  EXPECT_PRED_FORMAT2(StripWSisSubstring<>, ExpectedErr, ActualErr);
  EXPECT_STREQ("2173\n", ActualOut.c_str());
}
