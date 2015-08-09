// -*- C++ -*-
#ifndef RHINE_UNRESOLVEDVALUE_H
#define RHINE_UNRESOLVEDVALUE_H

#include "rhine/IR/User.h"

namespace rhine {
class UnresolvedValue : public User {
public:
  UnresolvedValue(std::string N, Type *T, RTValue ValID = RT_UnresolvedValue);
  virtual ~UnresolvedValue() {}
  static UnresolvedValue *get(std::string N, Type *T);
  static bool classof(const Value *V);
  virtual llvm::Value *toLL(llvm::Module *M) override;
protected:
  virtual void print(std::ostream &Stream) const override;
};

class Argument : public UnresolvedValue {
public:
  Argument(std::string N, Type *T);
  virtual ~Argument() {}
  static Argument *get(std::string N, Type *T, Context *K);
  static bool classof(const Value *V);
  virtual llvm::Value *toLL(llvm::Module *M) override;
protected:
  virtual void print(std::ostream &Stream) const override;
};
}

#endif
