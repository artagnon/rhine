//-*- C++ -*-

#ifndef RHINE_BASICBLOCK_H
#define RHINE_BASICBLOCK_H

#include "rhine/IR/Value.h"
#include <vector>

namespace rhine {
class Function;
class Module;

/// A BasicBlock is a container of instructions, and there are many BasicBlocks
/// in a function. FlattenBB transforms the nested mess into a flat structure.
class BasicBlock : public Value {
  Function *Parent;
  std::vector<BasicBlock *> Predecessors;
  std::vector<BasicBlock *> Successors;
public:
  std::vector<Value *> ValueList;

  // Standard methods
  BasicBlock(Type *Ty, std::vector<Value *> V);
  virtual ~BasicBlock();
  static BasicBlock *get(std::vector<Value *> V, Context *K);
  static bool classof(const Value *V);
  llvm::Value *toLL(llvm::Module *M);

  // Iterator over all the instructions in this BB
  typedef std::vector<Value *>::iterator value_iterator;
  value_iterator begin();
  value_iterator end();

  // Iterate over Predecessors and Successors of this BB
  typedef std::vector<BasicBlock *>::iterator bb_iterator;
  bb_iterator pred_begin();
  bb_iterator pred_end();
  iterator_range<bb_iterator> preds();
  bb_iterator succ_begin();
  bb_iterator succ_end();
  iterator_range<bb_iterator> succs();

  // Methods to add and remove a single predecessor or successor
  void addPredecessors(std::vector<BasicBlock *> Preds);
  void removePredecessor(BasicBlock *Pred);
  void addSuccessors(std::vector<BasicBlock *> Succs);
  void removeSuccessor(BasicBlock *Succ);

  // Proxy for std methods acting on ValueList
  unsigned size();
  Value *back();

  // Acessors to Parent function
  void setParent(Function *F);
  Function *getParent() const;

  // Quick method to grab unique predecessor (if any)
  BasicBlock *getUniquePredecessor() const;

protected:
  // std ostream writer, for debugging
  virtual void print(std::ostream &Stream) const;
};
}

#endif
