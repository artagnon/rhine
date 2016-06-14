#pragma once

namespace rhine {
class Value;
class User;

class Use {
  /// A Use is basically a linked list of Value wrappers.
  Value *Val;
  Use *Prev;
  Use *Next;
  unsigned DistToUser;

public:
  /// Dist is the distance to User; used to walk the Use chain; the User uses it
  /// to reference a particular operand.
  Use(unsigned Dist);

  /// Noop.
  virtual ~Use();

  /// Which operand # is this Use?
  unsigned getOperandNumber();
  void setOperandNumber(unsigned Num);
  void setVal(Value *Val_);
  User *getUser();
  Value *val() const;
  Value *operator=(Value *RHS);
  Value *operator->();
  operator Value *() const;

  /// Swap out the values in this and RHS.
  void swap(Use &RHS);

  /// Add this to the given UseList.
  void addToList(Use *&UseList);

  /// Remove any intermediate element from the list.
  void removeFromList();

  /// Set the Val of this Use.
  void set(Value *V);

  /// Call destructors of each of these Uses and delete the memory.
  static void zap(Use *Start, const Use *Stop, bool Del);

  /// So that Value and User can access my non-public members without
  /// inheritance.
  friend class Value;
  friend class User;
};
}
