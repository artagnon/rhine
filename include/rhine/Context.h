//-*- C++ -*-

#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/ADT/FoldingSet.h"

#include <cstdlib>
#include <map>

#include "rhine/IR/Type.h"
#include "rhine/IR/BasicBlock.h"
#include "rhine/Diagnostic/Diagnostic.h"

namespace rhine {
class Value;
class Externals;

class Context {
public:
  llvm::BumpPtrAllocator RhAllocator;
  llvm::FoldingSet<FunctionType> FTyCache;
  llvm::FoldingSet<PointerType> PTyCache;
  llvm::FoldingSet<IntegerType> ITyCache;
  UnType UniqueUnType;
  VoidType UniqueVoidType;
  BoolType UniqueBoolType;
  FloatType UniqueFloatType;
  StringType UniqueStringType;
  llvm::LLVMContext &LLContext;
  llvm::IRBuilder<> *Builder;
  DiagnosticPrinter *DiagPrinter;
  Externals *ExternalsCache;
  llvm::Function *CurrentFunction;

  Context(std::ostream &ErrStream = std::cerr):
      LLContext(llvm::getGlobalContext()),
      Builder(new llvm::IRBuilder<>(LLContext)),
      DiagPrinter(new DiagnosticPrinter(&ErrStream)),
      ExternalsCache(nullptr),
      CurrentFunction(nullptr) {}

  // The big free
  void releaseMemory() {
    RhAllocator.Reset();
    FTyCache.clear();
    ITyCache.clear();
  }

  class ResolutionMap {
    typedef std::map<std::string, Value *> BlockResolutionMap;
    std::map<BasicBlock *, BlockResolutionMap> FunctionResolutionMap;
    std::map<Value *, llvm::Value *> LoweringMap;
    Value *searchOneBlock(Value *Val, BasicBlock *Block);
  public:
    void add(Value *Val, BasicBlock *Block = nullptr);
    void add(Value *Val, llvm::Value *LLVal);
    Value *get(Value *Val, BasicBlock *Block = nullptr);
    llvm::Value *getl(Value *Val);
  };

  ResolutionMap Map;
};
}

#endif
