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
  std::vector<Instruction *> InstList;

  /// Standard methods
  BasicBlock(Type *Ty, std::string Name, std::vector<Instruction *> V);
  virtual ~BasicBlock();
  static BasicBlock *get(std::string Name, std::vector<Instruction *> V, Context *K);
  static bool classof(const Value *V);

  /// When adding instructions to the BasicBlock, it's our reposibility to make
  /// the different Parent pointers point to us
  template <typename T> void setAllInstructionParents(std::vector<T *> List);

  /// When the entry block is toLL()'ed, and a branch instruction is found, the
  /// branch instruction takes over the responsibility of lowering its case
  /// blocks _and_ the merge block with the phi
  virtual llvm::Value *toLL(llvm::Module *M) override;

  /// Special methods to generate just the BasicBlock, and lower just the
  /// ValueList (choosing last value after evaluating all values)
  llvm::BasicBlock *toContainerLL(llvm::Module *M);
  llvm::Value *toValuesLL(llvm::Module *M);

  /// Iterator over all the instructions in this BB
  typedef std::vector<Instruction *>::iterator value_iterator;
  value_iterator begin();
  value_iterator end();

  /// Iterate over Predecessors and Successors of this BB
  typedef std::vector<BasicBlock *>::iterator bb_iterator;
  bb_iterator pred_begin();
  bb_iterator pred_end();
  iterator_range<bb_iterator> preds();
  bb_iterator succ_begin();
  bb_iterator succ_end();
  iterator_range<bb_iterator> succs();

  /// Methods to add and remove a single predecessor or successor
  void addPredecessors(std::vector<BasicBlock *> Preds);
  void removePredecessor(BasicBlock *Pred);
  void addSuccessors(std::vector<BasicBlock *> Succs);
  void removeSuccessor(BasicBlock *Succ);

  /// Proxy for std methods acting on ValueList
  unsigned size();
  Instruction *back();

  template <class ForwardIterator, class T>
  void replace(ForwardIterator First, ForwardIterator Last,
               const T& OldValue, const T& NewValue);

  /// Acessors to Parent function
  void setParent(Function *F);
  Function *getParent() const;

  /// Quick method to check if Predecessors/Successors is empty
  bool hasNoPredecessors() const;
  bool hasNoSuccessors() const;

  /// Quick method to grab unique predecessor/successor (if any)
  BasicBlock *getUniquePredecessor() const;
  BasicBlock *getUniqueSuccessor() const;

protected:
  /// std ostream writer, for debugging
  virtual void print(std::ostream &Stream) const override;
};
}

#endif
