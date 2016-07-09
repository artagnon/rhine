#pragma once

#include "rhine/Diagnostic/Diagnostic.hpp"
#include "rhine/IR/User.hpp"

namespace rhine {
class GlobalString : public User {
  std::string Val;

public:
  GlobalString(std::string Val, Context *K);
  virtual ~GlobalString() = default;
  void *operator new(size_t s);
  static GlobalString *get(std::string Val, Context *K);
  static bool classof(const Value *V);
  std::string val();
  virtual llvm::Value *generate(llvm::Module *M) override;

protected:
  virtual void print(DiagnosticPrinter &Stream) const override;
};
}
