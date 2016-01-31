#ifndef RHINE_CONTEXT_H
#define RHINE_CONTEXT_H

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/ADT/FoldingSet.h"

#include <iostream>
#include <cstdlib>
#include <map>

#include "rhine/IR/Type.hpp"
#include "rhine/IR/Constant.hpp"

namespace rhine {
class Value;
class Externals;
class BasicBlock;
class DiagnosticPrinter;

class Context {
public:
  /// Type caches
  llvm::FoldingSet<FunctionType> FTyCache;
  llvm::FoldingSet<PointerType> PTyCache;
  llvm::FoldingSet<TensorType> TTyCache;
  llvm::FoldingSet<IntegerType> ITyCache;
  std::vector<UnType *> UTyCache;
  VoidType UniqueVoidType;
  BoolType UniqueBoolType;
  FloatType UniqueFloatType;
  StringType UniqueStringType;

  /// Constant caches
  llvm::FoldingSet<ConstantInt> CIntCache;
  llvm::FoldingSet<ConstantBool> CBoolCache;
  llvm::FoldingSet<ConstantFloat> CFltCache;
  llvm::FoldingSet<Pointer> PtrCache;

  /// The LLVM Context (initialized to llvm::getGlobalContext()), Builder
  /// (initalized to a new IRBuilder instance), DiagnosticPrinter (initialized
  /// to a new DiagPrinter instance), and ExternalsCache (initialized to
  /// nullptr).
  llvm::LLVMContext &LLContext;
  std::unique_ptr<llvm::IRBuilder<>> Builder;
  std::unique_ptr<DiagnosticPrinter> DiagPrinter;
  std::unique_ptr<Externals> ExternalsCache;

  /// Creates a new Builder and DiagPrinter. There should only be one Context
  /// per thread.
  Context(std::ostream &ErrStream = std::cerr);
  virtual ~Context();

  /// Symbol resolution map; should probably be per-Module?
  class ResolutionMap {
    typedef std::map<std::string, Value *> NameResolutionMap;
    std::map<BasicBlock *, NameResolutionMap> BlockResolutionMap;
    std::map<Value *, llvm::Value *> LoweringMap;
    Value *searchOneBlock(Value *Val, BasicBlock *Block);
  public:
    bool add(Value *Val, BasicBlock *Block = nullptr);
    bool add(Value *Val, llvm::Value *LLVal);
    Value *get(Value *Val, BasicBlock *Block = nullptr);
    llvm::Value *getl(Value *Val);
  };

  ResolutionMap Map;
};
}

#endif
