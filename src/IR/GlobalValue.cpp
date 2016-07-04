#include "rhine/IR/GlobalValue.hpp"
#include "rhine/IR/Type.hpp"

namespace rhine {
GlobalString::GlobalString(std::string Val, Context *K)
    : User(StringType::get(K), RT_GlobalString), Val(Val) {}

GlobalString::~GlobalString() {}

void *GlobalString::operator new(size_t s) { return User::operator new(s); }

GlobalString *GlobalString::get(std::string Val, Context *K) {
  return new GlobalString(Val, K);
}

bool GlobalString::classof(const Value *V) {
  return V->op() == RT_GlobalString;
}

std::string GlobalString::val() { return Val; }

void GlobalString::print(DiagnosticPrinter &Stream) const {
  Stream << "'" << Val << "' " << *type();
}
}
