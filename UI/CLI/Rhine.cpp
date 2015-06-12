#include "rhine/OptionParser.h"
#include "rhine/Toplevel/Toplevel.h"

#include <iostream>

enum OptionIndex {
  UNKNOWN,
  DEBUG,
  STDIN,
  HELP
};

const option::Descriptor Usage[] =
  {
    {UNKNOWN, 0, "", "", option::Arg::None,
     "USAGE: Rhine [options] <filename>\n\nOptions:"},
    {DEBUG, 0, "", "debug", option::Arg::None,
     " --debug  \tDebug lexer and parser"},
    {STDIN, 0, "", "stdin", option::Arg::None,
     " --stdin  \tRead input from stdin"},
    {HELP, 0, "", "help", option::Arg::None,
     " --help  \tPrint usage and exit"},
    {}
  };

int main(int argc, char *argv[]) {
  argc-=(argc>0); argv+=(argc>0); // skip program name argv[0] if present
  option::Stats  Stats(Usage, argc, argv);
  option::Option* Options = new option::Option[Stats.options_max];
  option::Option* Buffer  = new option::Option[Stats.buffer_max];
  option::Parser Parse(Usage, argc, argv, Options, Buffer);

  if (Parse.error())
    return 128;

  for (option::Option *Opt = Options[UNKNOWN]; Opt; Opt = Opt->next())
    std::cout << "Unknown options: " <<
      std::string(Opt->name, Opt->namelen) << std::endl;
  if (Options[UNKNOWN])
    return 128;

  if (Options[HELP]) {
    option::printUsage(std::cout, Usage);
    return 0;
  }

  if (Options[STDIN]) {
    std::string Input;
    for (std::string Line; std::getline(std::cin, Line);)
      Input += Line;
    auto FHandle = rhine::jitFacade(Input, Options[DEBUG], true);
    delete Options;
    delete Buffer;
    FHandle();
    return 0;
  }

  if (Parse.nonOptionsCount() != 1) {
    option::printUsage(std::cout, Usage);
    return 128;
  }

  auto FHandle = rhine::jitFacade(Parse.nonOption(0), Options[DEBUG]);
  delete Options;
  delete Buffer;
  FHandle();
  return 0;
}
