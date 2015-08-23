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
  Context *Kontext;
  std::vector<Function *> ContainedFs;
public:
  Module(Context *K);
  virtual ~Module() {}
  static Module *get(Context *K);
  Context *getContext();
  void appendFunction(Function *F) {
    F->setParent(this);
    ContainedFs.push_back(F);
  }
  void insertFunction(std::vector<Function *>::iterator It, Function *F) {
    F->setParent(this);
    ContainedFs.insert(It, F);
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
  virtual void toLL(llvm::Module *M);
  typedef std::vector<Function *>::iterator iterator;
  Function *front();
  iterator begin();
  iterator end();
  void dump();
protected:
  virtual void print(std::ostream &Stream) const {
    for (auto F: ContainedFs)
      Stream << *F << std::endl;
  }
};
}

#endif
