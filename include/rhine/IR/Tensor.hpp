#pragma once

#include <cstdlib>
#include <vector>

#include "rhine/IR/Type.hpp"
#include "rhine/IR/Value.hpp"

/// Forward declaration
namespace llvm {
class Value;
}

namespace rhine {
class Tensor : public Value {
  std::vector<size_t> Dimensions;
  std::vector<Value *> Elements;

public:
  /// A tensor is a finite, non-zero list of entries. Dims = { M, N, P }, and
  /// Elts = { ... MxNxP elements ... } for a MxNxP tensor, stored in row-major
  /// order. Dims and ContainedType is part of Ty.
  Tensor(Type *Ty, std::vector<Value *> Elts);

  /// Destructor is a no-op
  virtual ~Tensor() = default;

  /// Invokes sanity checks on Dims and Elts before calling constructor. Context
  /// is required to construct empty tensor.
  static Tensor *get(std::vector<size_t> Dims, std::vector<Value *> Elts,
                     Context *K = nullptr);

  TensorType *type() const;

  std::vector<Value *> getElts() const;

  /// RT_Tensor
  static bool classof(const Value *V);

  /// Intricate malloc + gep operations
  virtual llvm::Value *toLL(llvm::Module *M) override;

  /// Printed like Lua tables, { ... }
  virtual void print(DiagnosticPrinter &Stream) const override;
};
}
