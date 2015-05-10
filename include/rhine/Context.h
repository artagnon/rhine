//-*- C++ -*-

#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include "llvm/IR/Value.h"
#include "llvm/ADT/FoldingSet.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/DiagnosticIDs.h"
#include "clang/Basic/DiagnosticOptions.h"
#include "clang/Basic/FileManager.h"
#include "clang/Basic/LangOptions.h"
#include "clang/Frontend/TextDiagnosticPrinter.h"

#include <map>

using namespace clang;

namespace rhine {
class Context {
  // For function overloading, NameTypeMapping should map std::string to
  // std::vector<Symbol *>
  std::map <std::string, class Type *> NameTypeMapping;
  std::map <std::string, llvm::Value *> SymbolMapping;

  // Diagnostic stuff
  FileSystemOptions FileMgrOpts;
  FileManager FileMgr;
  DiagnosticOptions *DiagOpts;
  TextDiagnosticPrinter *DiagClient;
public:
  llvm::BumpPtrAllocator RhAllocator;
  llvm::FoldingSet<class Symbol> SymbolCache;
  llvm::FoldingSet<class FunctionType> FTyCache;
  SourceManager SourceMgr;
  DiagnosticsEngine Diags;
  FileID MainFileID;

  Context(std::string *Filename, std::string *PrgString = nullptr) :
      FileMgr(FileMgrOpts), DiagOpts(new DiagnosticOptions),
      DiagClient(new TextDiagnosticPrinter(llvm::errs(), DiagOpts)),
      SourceMgr(Diags, FileMgr),
      Diags(new DiagnosticIDs, DiagOpts, DiagClient)
  {
    DiagOpts->ShowColors = true;
    DiagClient->BeginSourceFile(LangOptions(), nullptr);
    if (Filename) {
      const FileEntry *File = FileMgr.getFile(*Filename, /*OpenFile=*/true);
      MainFileID = SourceMgr.createFileID(File, SourceLocation(), SrcMgr::C_System);
    } else {
      std::unique_ptr<llvm::MemoryBuffer> Buf =
        llvm::MemoryBuffer::getMemBuffer(*PrgString);
      MainFileID = SourceMgr.createFileID(std::move(Buf));
    }
    SourceMgr.setMainFileID(MainFileID);
  }

  // The big free
  void releaseMemory() {
    RhAllocator.Reset();
    SymbolCache.clear();
    FTyCache.clear();
    NameTypeMapping.clear();
    SymbolMapping.clear();
  }

  //===--------------------------------------------------------------------===//
  // Functions that operate on SymbolMapping.
  //===--------------------------------------------------------------------===//
  bool addMapping(std::string S, llvm::Value *V) {
    if (SymbolMapping.find(S) != SymbolMapping.end())
      return false;
    SymbolMapping.insert(std::make_pair(S, V));
    return true;
  }
  llvm::Value *getMapping(std::string S) {
    auto V = SymbolMapping.find(S);
    return V == SymbolMapping.end() ? nullptr : V->second;
  }
  llvm::Value *getMappingOrDie(std::string S) {
    auto V = SymbolMapping.find(S);
    assert(V != SymbolMapping.end() && "Unbound symbol");
    return V->second;
  }
  //===--------------------------------------------------------------------===//
  // Functions that operate on NameTypeMapping.
  //===--------------------------------------------------------------------===//
  bool addNameTypeMapping(std::string N, class Type *T) {
    if (NameTypeMapping.find(N) != NameTypeMapping.end())
      return false;
    NameTypeMapping.insert(std::make_pair(N, T));
    return true;
  }
  Type *getNameTypeMappingOrDie(std::string S) {
    auto V = NameTypeMapping.find(S);
    assert(V != NameTypeMapping.end() &&
           "internal error: NameTypeMapping not pre-populated");
    return V->second;
  }
};
}

#endif
