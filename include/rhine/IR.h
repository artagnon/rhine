//-*- C++ -*-

#ifndef IR_H
#define IR_H

#include "llvm/IR/Module.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/TypeBuilder.h"
#include "llvm/IR/Constants.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/STLExtras.h"

#include <string>
#include <vector>
#include <sstream>

#include "location.hh"
#include "rhine/Context.h"
#include "rhine/IR/Type.h"
#include "rhine/IR/Value.h"
#include "rhine/IR/Instruction.h"
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
protected:
  virtual void print(std::ostream &Stream) const {
    for (auto F: ContainedFs)
      Stream << *F << std::endl;
  }
};
}

#endif
