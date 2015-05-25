// -*- C++ -*-
#ifndef RHINE_RESOLVE_H
#define RHINE_RESOLVE_H

#include "rhine/Context.h"
#include "rhine/Externals.h"
#include <string>

namespace rhine {
class Resolve {
public:
  static class Type *resolveSymbolTy(std::string Name, class Type *Ty, Context *K) {
    if (!isa<UnType>(Ty))
      return Ty;
    if (auto Result = K->getMappingTy(Name)) {
      return Result;
    } else if (auto Result = Externals::get(K)->getMappingTy(Name)) {
      return Result;
    }
    return nullptr;
  }
};
}

#endif
