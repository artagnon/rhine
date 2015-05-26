//-*- C++ -*-

#ifndef MODULE_H
#define MODULE_H

#include "llvm/IR/Module.h"

#include <vector>
#include <sstream>

#include "rhine/Context.h"
#include "rhine/IR/Constant.h"

using namespace std;
using namespace llvm;

namespace rhine {
class Module {
  std::vector<Function *> ContainedFs;
public:
  Module *get() {
    return new Module;
  }
  void appendFunction(Function *F) {
    ContainedFs.push_back(F);
  }
  std::vector<Function *> getVal() {
    return ContainedFs;
  }
  void setVal(std::vector<Function *> Fs) {
    ContainedFs = Fs;
  }
  friend ostream &operator<<(ostream &Stream, const Module &M) {
    M.print(Stream);
    return Stream;
  }
  void typeInfer(Context *K = nullptr);
  void toLL(llvm::Module *M, Context *K);
  typedef std::vector<Function *>::iterator iterator;
  iterator begin() {
    return ContainedFs.begin();
  }
  iterator end() {
    return ContainedFs.end();
  }
protected:
  virtual void print(std::ostream &Stream) const {
    for (auto F: ContainedFs)
      Stream << *F << std::endl;
  }
};
}

#endif
