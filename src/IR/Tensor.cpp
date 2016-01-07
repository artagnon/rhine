#include <cstdlib>
#include <vector>

#include "rhine/IR/Type.hpp"
#include "rhine/IR/Value.hpp"
#include "rhine/IR/Tensor.hpp"

namespace rhine {
Tensor::Tensor(Type *Ty, std::vector<size_t> Dims, std::vector<Value *> Elts)
    : Value(Ty, RT_Tensor), Dimensions(Dims), Elements(Elts) {}

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
  auto Ty = Elts.size() ? Elts[0]->getType() : UnType::get(K);
  for (auto Elt : Elts) {
    assert(Elt->getType() == Ty && "Inconsistent tensor type");
  }
  return new Tensor(Ty, Dims, Elts);
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
