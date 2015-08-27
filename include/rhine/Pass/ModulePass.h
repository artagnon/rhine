//-*- C++ -*-
#ifndef RHINE_MODULETRANSFORM_H
#define RHINE_MODULETRANSFORM_H

namespace rhine {
class Module;
class Function;

class ModulePass {
public:
  virtual ~ModulePass() {}
  virtual void runOnModule(Module *M) = 0;
private:
  virtual void runOnFunction(Function *F) = 0;
};
}

#endif
