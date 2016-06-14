#include "rhine/IR/Module.hpp"
#include "rhine/IR/Context.hpp"

namespace rhine {
Module::Module(std::unique_ptr<Context> K) : Kontext(std::move(K)) {}

Module::~Module() {
  dropAllReferences();
  for (auto &F : ContainedFs)
    delete F;
  ContainedFs.clear();
}

void Module::dropAllReferences() {
  for (auto &F : *this)
    F->dropAllReferences();
}

Module *Module::get(std::unique_ptr<Context> K) {
  return new Module(std::move(K));
}

Context *Module::getContext() { return Kontext.get(); }

void Module::appendFunction(Function *F) {
  F->setParent(this);
  ContainedFs.push_back(F);
}

void Module::insertFunction(std::vector<Function *>::iterator It, Function *F) {
  F->setParent(this);
  ContainedFs.insert(It, F);
}

std::vector<Function *> Module::val() const {
  return ContainedFs;
}

void Module::setVal(std::vector<Function *> Fs) {
  ContainedFs = Fs;
}

Function *Module::front() {
  return ContainedFs.front();
}

Function *Module::back() {
  return ContainedFs.back();
}

Function *Module::find(std::string FunctionName) {
  for (auto &F : *this) {
    if (F->getName() == FunctionName)
      return F;
  }
  return nullptr;
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
