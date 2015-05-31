#include "rhine/Toplevel/Toplevel.h"
#include "rhine/Toplevel/ParseFacade.h"

#include <iostream>
#include <sstream>

using namespace llvm;

namespace rhine {
MainFTy jitFacade(std::string InStr, bool Debug) {
  auto Pf = ParseFacade(InStr, std::cerr, nullptr, Debug);
  return Pf.jitAction(ParseSource::FILE, PostParseAction::LLDUMP);
}
}
