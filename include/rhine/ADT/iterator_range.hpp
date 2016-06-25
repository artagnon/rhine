#pragma once

#include <utility>

namespace rhine {
template <typename IteratorT> class iterator_range {
  IteratorT begin_iterator, end_iterator;

public:
  iterator_range(IteratorT begin, IteratorT end)
      : begin_iterator(std::move(begin)), end_iterator(std::move(end)) {}
  IteratorT begin() { return begin_iterator; }
  IteratorT end() { return end_iterator; }
};

template <typename T> iterator_range<T> make_range(T x, T y) {
  return iterator_range<T>(std::move(x), std::move(y));
}
};
