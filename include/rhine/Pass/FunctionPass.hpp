#pragma once

#include "rhine/Pass/ModulePass.hpp"

namespace rhine {
class Module;
class Function;

class FunctionPass : public ModulePass {
public:
  void runOnModule(Module *M);
  virtual void runOnFunction(Function *F) = 0;
};
}
