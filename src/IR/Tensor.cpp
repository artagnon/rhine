#include <cstdlib>
#include <vector>

#include "rhine/IR/Value.hpp"
#include "rhine/IR/Tensor.hpp"

namespace rhine {
Tensor::Tensor(std::vector<size_t> Dims, std::vector<Value *> Elts)
    : Value(Elts[0]->getType(), RT_Tensor), Dimensions(Dims), Elements(Elts) {}

Tensor::~Tensor() {}

Tensor *Tensor::get(std::vector<size_t> Dims, std::vector<Value *> Elts) {
  assert(Elts.size() && "Attempting to create empty tensor");
  size_t DimAccumulator = 1;
  for (auto Dim : Dims) {
    DimAccumulator *= Dim;
  }
  assert(Elts.size() == DimAccumulator &&
         "Mismatched dimensions and number of elements in tensor");
  auto Ty = Elts[0]->getType();
  for (auto Elt : Elts) {
    assert(Elt->getType() == Ty && "Inconsistent tensor type");
  }
  return new Tensor(Dims, Elts);
}

bool Tensor::classof(const Value *V) {
  return V->getValID() == RT_Tensor;
}

void Tensor::print(DiagnosticPrinter &Stream) const {
  Stream << "{ ";
  for (auto Elt : Elements) {
    Stream << Elt;
  }
  Stream << " }" << std::endl;
}
}
