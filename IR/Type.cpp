#include "rhine/IR/Type.h"
#include <iostream>

namespace rhine {
__attribute__((used, noinline))
void Type::dump() {
  std::cout << *this << std::endl;
}

RTType Type::getTyID() const { return TyID; }

void Type::setSourceLocation(location SrcLoc) {
  SourceLoc = SrcLoc;
}

location Type::getSourceLocation() {
  return SourceLoc;
}
}
