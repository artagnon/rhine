#include "rhine/IR/Module.h"

namespace rhine {
Module::Module(Context *K) : Kontext(K) {}

Module *Module::get(Context *K) {
  return new (K->RhAllocator) Module(K);
}

Context *Module::getContext() { return Kontext; }

void Module::appendFunction(Function *F) {
  F->setParent(this);
  ContainedFs.push_back(F);
}

void Module::insertFunction(std::vector<Function *>::iterator It, Function *F) {
  F->setParent(this);
  ContainedFs.insert(It, F);
}

std::vector<Function *> Module::getVal() const {
  return ContainedFs;
}

void Module::setVal(std::vector<Function *> Fs) {
  ContainedFs = Fs;
}

Function *Module::front() {
  return ContainedFs.front();
}

Module::iterator Module::begin() {
  return ContainedFs.begin();
}

Module::iterator Module::end() {
  return ContainedFs.end();
}

__attribute__((used, noinline))
void Module::dump() {
  std::cout << *this << std::endl;
}
}
