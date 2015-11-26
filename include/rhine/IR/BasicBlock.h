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
  std::vector<Value *> StmList;

  /// Standard methods
  BasicBlock(Type *Ty, std::string Name, std::vector<Value *> V);
  virtual ~BasicBlock();
  static BasicBlock *get(std::string Name, std::vector<Value *> V, Context *K);
  static bool classof(const Value *V);

  /// The function's responsibility is simply to codegen the EntryBlock. A block
  /// with a terminator instruction (i.e. every block) will codegen other blocks
  /// referenced by the terminator. So, an IfInst codegens the TrueBlock, the
  /// FalseBlock, and the MergeBlock.
  virtual llvm::Value *toLL(llvm::Module *M) override;

  /// Special methods to generate just the BasicBlock, and lower just the
  /// ValueList (choosing last value after evaluating all values)
  llvm::BasicBlock *toContainerLL(llvm::Module *M);
  llvm::Value *toValuesLL(llvm::Module *M);

  /// Iterator over all the instructions in this BB
  typedef std::vector<Value *>::iterator value_iterator;
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
  Value *back();

  template <class ForwardIterator, class T>
  void replace(ForwardIterator First, ForwardIterator Last, const T &OldValue,
               const T &NewValue);

  /// Acessors to Parent function
  void setParent(Function *F);
  Function *getParent() const;

  /// Quick method to check if Predecessors/Successors is empty
  bool hasNoPredecessors() const;
  bool hasNoSuccessors() const;

  /// Get the predecessor/successor sizes
  unsigned pred_size() const;
  unsigned succ_size() const;

  /// Quick method to grab unique predecessor/successor (if any)
  BasicBlock *getUniquePredecessor() const;
  BasicBlock *getUniqueSuccessor() const;

  /// Keep digging through one successor edge until we tally
  /// successor/predecessor count and find the block containing the phi node
  /// corresponding to the branch instruction in this block. If the last
  /// instruction in this block is not a branch, return nullptr.
  BasicBlock *getMergeBlock();

  /// Given a Block with a known branch instruction, codegen it.
  llvm::Value *getPhiValueFromBranchBlock(llvm::Module *M);

private:
  std::vector<std::pair<BasicBlock *, llvm::BasicBlock *>>
  zipSuccContainers(llvm::Module *M);

protected:
  /// std ostream writer, for debugging
  virtual void print(std::ostream &Stream) const override;
};
}

#endif
