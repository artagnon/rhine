#pragma once

#include "rhine/IR/Value.hpp"
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
  std::vector<Instruction *> InstList;

public:
  /// The name is just a hint: we don't actually uniquify names; llvm does.
  BasicBlock(Type *Ty, std::string Name, std::vector<Instruction *> V);

  /// Delete all the instructions, after dropping references to all its uses. In
  /// the case of a branch instruction, the _references_ to the TrueBB and
  /// FalseBB will be dropped, but those BasicBlocks won't be deleted until the
  /// parent that iterates over BasicBlocks deletes them.
  virtual ~BasicBlock();

  /// Pass through to each Instruction's dropAllReferences().
  void dropAllReferences();

  static BasicBlock *get(std::string Name, std::vector<Instruction *> V,
                         Context *K);
  static bool classof(const Value *V);

  /// Just take out the instruction list as a reference, and do whatever with
  /// it; we don't provide functions to manipulate it.
  std::vector<Instruction *> &getInstList();

  /// A block with a terminator instruction (i.e. every block) will codegen
  /// other blocks referenced by the terminator. So, an IfInst codegens the
  /// TrueBlock and the FalseBlock, but not the MergeBlock. We codegen just the
  /// current block, and the corresponding MergeBlock, if it exists.
  ///
  /// def main do
  ///   print 'entryBlock'     < * EntryBlock Inst 1
  ///   if (1) do              < * EntryBlock Inst 2 (codegens TrueBB, FalseBB)
  ///     print 'trueBranch'   < TrueBB Inst 1
  ///   else
  ///     print 'falseBranch'  < FalseBB Inst 1
  ///   end
  ///   print 'mergeBranch'    < * MergeBlock Inst 1
  /// end
  ///
  /// The instructions marked with asterisks are codegened directly by us.
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

  /// Methods to add and remove single/multiple precessors/successors.
  void addPredecessors(std::vector<BasicBlock *> Preds);
  void setPredecessors(std::vector<BasicBlock *> Preds);
  void removePredecessor(BasicBlock *Pred);
  void addSuccessors(std::vector<BasicBlock *> Succs);
  void setSuccessors(std::vector<BasicBlock *> Succs);
  void removeSuccessor(BasicBlock *Succ);

  /// Proxy for std methods acting on ValueList
  unsigned size();
  Instruction *back();

  /// Search for the Instruction that binds SymbolName named symbol
  Instruction *find(std::string SymbolName);

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
  virtual void print(DiagnosticPrinter &Stream) const override;
};
}
