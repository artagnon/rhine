// -*- C++ -*-
#ifndef GLOBALVALUE_H
#define GLOBALVALUE_H

#include "rhine/IR/User.h"

namespace rhine {
class GlobalString : public User {
  std::string Val;
public:
  GlobalString(std::string Val, Context *K);
  static GlobalString *get(std::string Val, Context *K);
  static bool classof(const Value *V);
  std::string getVal();
  virtual llvm::Value *toLL(llvm::Module *M);
protected:
  virtual void print(std::ostream &Stream) const override;
};
}

#endif
