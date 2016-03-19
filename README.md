# Rhine: An optionally-typed Elixir

Rhine is designed to be a fast language utilizing the LLVM JIT featuring N-d
tensors, first-class functions, optional typing.

Rhine started off as [rhine-ml](https://github.com/artagnon/rhine-ml), and
rhine-ml was called Rhine earlier.

Effort put into rhine-ml: 2 months
Effort put into Rhine: 1 year, 5 months

## Language Features

```elixir
def fptrTest do
  return println
end
def mathTest(Input ~Int, Finput ~Float) do
  PhiAssignment = if true do
    println $ 3 + Input
    2
  else
    A = {{2}, {3}}
    println A[0][1]
    Lam = fn Arg -> Arg * 2 end
    fptrTest (Lam 4)
    3
  end
  ret PhiAssignment
end
```

rhine-ml has arrays, first-class functions, closures, variadic arguments,
macros. Rhine has N-d tensors, first-class functions, more syntactic sugar, and
reports better errors.

Rhine is typed while rhine-ml is not. As a result, Rhine does full type
inference, and rhine-ml gets away with boxing-unboxing.

## The recursive-descent parser

Rhine uses a handwritten recursive-descent parser, which is faster and reports
better errors, than the former Bison one. You will need to use a one-token
lookahead atleast, if you want to keep the code simple. This gives you one level
of:

```cpp
parseSymbol(); // Oops, the lexed token indicates that we're not in the right
               // function

parseInstruction(); // Ask it to use an existing token, not lex a new one
```

## The AST

The AST is heavily inspired by LLVM IR, although it has some higher-level
concepts like `Tensor`. It's an SSA and has a UseDef graph embedded in it,
making analysis and transformation easy.

The main classes are `Type` and `Value`. All types like `IntType`, `FloatType`
inherit from `Type`, most of the others inherit from `Value`. A `BasicBlock` is
a `Value`, and so is `ConstantInt`.

A `BasicBlock` is a vector of `Instruction`, and this is how the AST is an SSA:
assignments are handled as a `StoreInst` which takes an `UnresolvedValue` and
the `Value` as arguments.

## UseDef in AST

```cpp
/// A Use is basically a linked list of Value wrappers
class Use {
  Value \*Val;
  Use \*Prev;
  Use \*Next;
   // Laid out in memory as [User] - [Use1] - [Use2]. Use2 has DistToUser 2
  unsigned DistToUser;
};

/// A User is an Instruction. Instruction inherits from User
class User : public Value {
protected:
  /// In the case of CallInst, the Callee itself is operand 0, which
  /// NumAllocatedOps accounts for, and NumOperands omits. See corresponding
  /// iterators uses() and operands().
  unsigned NumOperands;
  unsigned NumAllocatedOps;
public:
  /// Allocate memory for Us uses too.
  void \*operator new(size_t Size, unsigned Us) {
    void \*Storage = ::operator new (Us \* sizeof(Use) + Size);
    auto Start = static_cast<Use \*>(Storage);
    auto End = Start + Us;
    for (unsigned Iter = 0; Iter < Us; Iter++) {
      new (Start + Iter) Use(Us - Iter);
    }
    auto Obj = reinterpret_cast<User *>(End);
    return Obj;
  }

};
class Instruction : User;
```

## Symbol resolution

Transform/Resolve is an example of something that utilizes the UseDef embedded
in the AST.

```elixir
  B = A + 2
```

creates one `UnresolvedValue`, `A`, an `AddInst`, and a `MallocInst`,
which takes the string "B" and `AddInst` as operands.

The transform basically goes over all the `Instruction` in the `BasicBlock`,
resolves `UnresolvedValue` instances, and sets the `Use` to the resolved value.
It hence replaces every `Value` instance corresponding to a `Use` in one stroke.

## Lessons learnt

- C++ is suitable only for teams with a lot of manpower.

- [Crystal](http://crystal-lang.org/) made a good decision to start with Ruby.
  If your idea is to self-host, then the original language's efficiency does not
  matter. All you need is good generated assembly (which LLVM makes easy).

-
