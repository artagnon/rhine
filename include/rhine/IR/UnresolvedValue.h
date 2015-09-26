// -*- C++ -*-
#ifndef RHINE_UNRESOLVEDVALUE_H
#define RHINE_UNRESOLVEDVALUE_H

#include "rhine/IR/User.h"

namespace rhine {
class Prototype;

class UnresolvedValue : public User {
public:
  UnresolvedValue(std::string N, Type *T);
  virtual ~UnresolvedValue() {}
  static UnresolvedValue *get(std::string N, Type *T);
  static bool classof(const Value *V);
  virtual llvm::Value *toLL(llvm::Module *M) override;
protected:
  virtual void print(std::ostream &Stream) const override;
};

class Argument : public User {
  Prototype *Parent;
public:
  Argument(std::string N, Type *T);
  virtual ~Argument() {}
  static Argument *get(std::string N, Type *T);
  static bool classof(const Value *V);
  virtual llvm::Value *toLL(llvm::Module *M) override;
  void setParent(Prototype *P);
protected:
  virtual void print(std::ostream &Stream) const override;
};
}

#endif
