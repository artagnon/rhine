#pragma once

#include "rhine/IR/User.hpp"

namespace rhine {
class Prototype;

class UnresolvedValue : public User {
public:
  UnresolvedValue(std::string N, Type *T);
  virtual ~UnresolvedValue();
  static UnresolvedValue *get(std::string N, Type *T);
  static bool classof(const Value *V);
  virtual llvm::Value *toLL(llvm::Module *M) override;
protected:
  virtual void print(DiagnosticPrinter &Stream) const override;
};

class Argument : public User {
  Prototype *Parent;
public:
  Argument(std::string N, Type *T);
  virtual ~Argument();
  static Argument *get(std::string N, Type *T);
  static bool classof(const Value *V);
  virtual llvm::Value *toLL(llvm::Module *M) override;
  void setParent(Prototype *P);
protected:
  virtual void print(DiagnosticPrinter &Stream) const override;
};
}
