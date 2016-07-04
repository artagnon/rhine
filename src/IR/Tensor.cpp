#include <cstdlib>
#include <vector>

#include "rhine/IR/Type.hpp"
#include "rhine/IR/Value.hpp"
#include "rhine/IR/Tensor.hpp"

namespace rhine {
Tensor::Tensor(Type *Ty, std::vector<Value *> Elts)
    : Value(Ty, RT_Tensor), Elements(Elts) {}

Tensor::~Tensor() {}

Tensor *Tensor::get(std::vector<size_t> Dims, std::vector<Value *> Elts,
                    Context *K) {
  size_t DimAccumulator = 1;
  for (auto Dim : Dims) {
    DimAccumulator *= Dim;
  }
  assert(Elts.size() == DimAccumulator &&
         "Mismatched dimensions and number of elements in tensor");
  assert(Elts.size() || K && "Context not supplied for empty tensor");
  auto Ty = Elts.size() ? Elts[0]->type() : IntegerType::get(32, K);
  for (auto Elt : Elts) {
    if (Elt->type() != Ty) {
      assert (0 && "Inconsistent tensor type");
    }
  }
  return new Tensor(TensorType::get(Ty, Dims), Elts);
}

TensorType *Tensor::type() const {
  return cast<TensorType>(Value::type());
}

std::vector<Value *> Tensor::getElts() const {
  return Elements;
}

bool Tensor::classof(const Value *V) {
  return V->op() == RT_Tensor;
}

void Tensor::print(DiagnosticPrinter &Stream) const {
  Stream << "{ ";
  if (Elements.size()) {
    Stream << *Elements[0];
    for (auto It = Elements.begin(); std::next(It) != Elements.end(); It++) {
      Value *Elt = *It;
      Stream << ", " << *Elt;
    }
  }
  Stream << " } ~" << *type() << std::endl;
}
}
