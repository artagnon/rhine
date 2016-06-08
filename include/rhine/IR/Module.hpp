#pragma once

#include <sstream>
#include <vector>

#include "rhine/IR/Function.hpp"

using namespace std;
using namespace llvm;

namespace rhine {
class Context;

class Module {
  std::unique_ptr<Context> Kontext;
  std::vector<Function *> ContainedFs;

public:
  /// Context is all that is required to initialize
  Module(std::unique_ptr<Context> K);

  /// Destructor gets rid of ContainedFs, but leaves Kontext alone
  virtual ~Module();

  /// Passes through to each Function's dropAllReferences().
  void dropAllReferences();

  static Module *get(std::unique_ptr<Context> K);
  Context *getContext();

  /// Append, or insert given an iterator
  void appendFunction(Function *F);
  void insertFunction(std::vector<Function *>::iterator It, Function *F);

  /// Get or set ContainedFs
  std::vector<Function *> getVal() const;
  void setVal(std::vector<Function *> Fs);

  /// Implemented by print
  friend ostream &operator<<(ostream &Stream, const Module &M) {
    M.print(Stream);
    return Stream;
  }
  virtual void toLL(llvm::Module *M);
  typedef std::vector<Function *>::iterator iterator;

  /// std methods that pass through to the underlying ContainedFs
  Function *front();
  Function *back();

  /// Find a function in the mess
  Function *find(std::string Name);

  iterator begin();
  iterator end();

  /// While in lldb
  void dump();

protected:
  /// std ostream writer, for debugging
  virtual void print(std::ostream &Stream) const {
    for (auto F : ContainedFs)
      Stream << *F << std::endl;
  }
};
}
