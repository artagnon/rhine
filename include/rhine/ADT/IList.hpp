#pragma once

#include <cassert>
#include <iterator>

namespace rhine {
class Value;
class User;

/// Intrusive doubly-linked list that forms the basis of most IR lists in rhine.
template <typename NodeTy> class IListNode {
public:
  IListNode() : Prev(nullptr), Next(nullptr) {}
  IListNode(NodeTy *P, NodeTy *N) : Prev(P), Next(N) {}
  NodeTy *prev() { return Prev; }
  NodeTy *next() { return Next; }
  void setPrev(NodeTy *P) { Prev = P; }
  void setNext(NodeTy *N) { Next = N; }

private:
  NodeTy *Prev;
  NodeTy *Next;
};

template <typename NodeTy>
class IListIterator
    : public std::iterator<std::bidirectional_iterator_tag, NodeTy> {
public:
  /// Iterator requires a IListNode for initialization.
  IListIterator(NodeTy *N) : Node(N) {}
  IListIterator(NodeTy &NR) : Node(&NR) {}
  IListIterator() : Node(nullptr) {}

  /// Necessary for std::next
  const IListIterator &operator=(const IListIterator<NodeTy> &Other) {
    Node = Other.Node;
    return *this;
  }

  /// Simple comparison.
  bool operator==(const IListIterator<NodeTy> &Other) const {
    return Node == Other.Node;
  }

  /// Necessary to compare with end()
  bool operator!=(const IListIterator<NodeTy> &Other) const {
    return Node != Other.Node;
  }

  /// Some NodeTy * operators.
  explicit operator NodeTy *() const { return Node; }
  NodeTy *operator*() const { return Node; }
  NodeTy *operator->() const { return operator*(); }

  /// Required for any range-based for.
  IListIterator &operator++() {
    assert(Node && "Incrementing off end of IList");
    Node = Node->next();
    return *this;
  }

  /// We're bi-directional.
  IListIterator &operator--() {
    assert(Node && "Decrementing off beginning of IList");
    Node = Node->prev();
    return *this;
  }

  /// Post-increment.
  IListIterator operator++(int) {
    auto tmp = *this;
    ++*this;
    return tmp;
  }

  /// Post-decrement.
  IListIterator operator--(int) {
    auto tmp = *this;
    --*this;
    return tmp;
  }

  /// Nifty.
  bool operator!() { return !Node; }

private:
  NodeTy *Node;
};

template <typename NodeTy> class IPList {
public:
  typedef IListIterator<NodeTy> iterator;

  /// When starting a BasicBlock (for example), list of Instructions is Sentinel
  /// to begin with.
  IPList() : Head(createSentinel()) {}

  /// Closely tied to the iterator.
  iterator begin() const { return iterator(Head); }

  /// If there's a sentinel Head, we return it. Otherwise, Head->prev() (since
  /// this is a circular linked list).
  iterator end() const {
    return headIsSentinel() ? iterator(Head) : iterator(Head->prev());
  }

  /// Classic insertion. headIsSentinel() is handled here.
  iterator insertAfter(iterator Where, NodeTy *NewEl) {
    auto HeadIsSentinel = headIsSentinel();
    if (HeadIsSentinel) {
      assert(Where == *Head && "Invalid insertion point in insertAfter");
    }
    auto OldNext = HeadIsSentinel ? Head : Where->next();
    Where->setNext(NewEl);
    NewEl->setPrev(*Where);
    NewEl->setNext(OldNext);
    OldNext->setPrev(NewEl);
    if (HeadIsSentinel) {
      Head = NewEl;
    }
    assert(Head->prev()->next() == Head);
    return Where->next();
  }

  /// insertAfter, when you don't want to provide the end of the linked list.
  /// O(1) insertion because this is a circular linked-list.
  iterator append(NodeTy *NewEl) {
    // Skip sentinel
    return insertAfter(headIsSentinel() ? Head : Head->prev()->prev(), NewEl);
  }

  /// Like std::vector::erase, but O(1)
  void erase(iterator Start, iterator End) {
    if (!Start) {
      return;
    }
    Start->prev()->setNext(*End);
  }

  /// Like std::vector::clear
  void clear() { Head = createSentinel(); }

  /// Like std::vector::size
  unsigned size() {
    unsigned Sz = 0;
    for (auto Elt : *this) {
      assert(Elt);
      ++Sz;
    }
    return Sz;
  }

  /// The end() of the circular linked-list, also Head->prev()
  inline constexpr NodeTy *createSentinel() const {
    return const_cast<NodeTy *>(static_cast<const NodeTy *>(&Sentinel));
  }

  /// operator! only works with real nullptr. Sentinel is a special value.
  bool headIsSentinel() const { return !Head->prev(); }

  /// Like std::vector::back
  NodeTy *back() {
    assert(Head->prev() && "Calling back() on empty IPList");
    return Head->prev()->prev();
  }

  /// Like std::vector::front
  NodeTy *front() {
    assert(Head->prev() && "Calling front() on empty IPList");
    return Head;
  }

private:
  /// The head of the circular linked-list. Tail is Head->prev()->prev()
  NodeTy *Head;

  /// The special end() value of all Iterators. Is either Head, if the list is
  /// empty, or Head->prev() otherwise.
  IListNode<NodeTy> Sentinel;
};
}
