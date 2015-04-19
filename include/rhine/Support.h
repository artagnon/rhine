//-*- C++ -*-

#ifndef SUPPORT_H
#define SUPPORT_H

#include <iostream>
#include "llvm/IR/Module.h"
#include "rhine/IR.h"

namespace rhine {
//===--------------------------------------------------------------------===//
// Meant to be used by unittests.
//===--------------------------------------------------------------------===//
std::string irToPP (Value *V);
std::string irToPP (Module *M);
std::string llToPP (llvm::Value *Obj);
std::string llToPP(llvm::Module *M);

std::string parseTransformIR(std::string PrgString,
                             std::ostream &ErrStream = std::cerr,
                             bool Debug = false);
std::string parseCodeGenString(std::string PrgString,
                               llvm::Module *M,
                               std::ostream &ErrStream = std::cerr,
                               bool Debug = false);
std::string parseCodeGenString(std::string PrgString,
                               std::ostream &ErrStream = std::cerr,
                               bool Debug = false);

//===--------------------------------------------------------------------===//
// Meant to be used by normal codepath: jitFacade.
//===--------------------------------------------------------------------===//
void parseCodeGenFile(std::string Filename, llvm::Module *M, bool Debug = false);
}

#endif
