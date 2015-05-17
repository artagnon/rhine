//-*- C++ -*-
#ifndef VALUE_H
#define VALUE_H

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
#include "rhine/Type.h"

using namespace std;
using namespace llvm;

namespace rhine {
enum RTValue {
  RT_Symbol,
  RT_GlobalString,
  RT_ConstantInt,
  RT_ConstantBool,
  RT_ConstantFloat,
  RT_Function,
  RT_AddInst,
  RT_CallInst,
  RT_BindInst,
};
}

#endif
