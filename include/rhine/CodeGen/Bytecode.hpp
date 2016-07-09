#pragma once

#include <sstream>

namespace rhine {
class Bytecode {
  std::stringstream generate();
  void write();
  void emitOpcode();
};
}
