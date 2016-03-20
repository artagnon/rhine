# rhine: A typed Elixir-inspired language on LLVM

rhine is designed to be a fast language utilizing the LLVM JIT featuring N-d
tensors, first-class functions, and type inference; specifying argument
types is enough. It has a full blown AST into which it embeds a UseDef graph.

rhine started off as [rhine-ml](https://github.com/artagnon/rhine-ml), and
rhine-ml was called rhine earlier.

- Effort put into rhine-ml: 2 months
- Effort put into rhine: 1 year, 5 months

## Language Features

```elixir
def bar(arithFn ~Function(Int -> Int -> Int)) do
  println $ arithFn 2 4
end
def addCandidate(A ~Int, B ~Int) do
  ret $ A + B
end
def subCandidate(C ~Int, D ~Int) do
  ret $ C - D
end
def main() do
  if false do
    bar addCandidate
  else
    bar subCandidate
  end
  A = {{2}, {3}}
  println A[1][0]
end
```

rhine-ml, in contrast, has arrays, first-class functions, closures, variadic
arguments, macros. It's also much less buggy.

## The recursive-descent parser

rhine uses a handwritten recursive-descent parser, which is faster and reports
better errors, than the former Bison one. You will need to use a one-token
lookahead atleast, if you want to keep the code simple. This gives you one level
of:

```cpp
parseSymbol(); // Oops, the lexed token indicates that we're not in the right
               // function

parseInstruction(); // Ask it to use an existing token, not lex a new one
```

Another minor consideration is that newlines must be handled explicitly if you
want to substitute ; with a newline in the language.

```cpp
void Parser::getTok() {
  LastTok = CurTok;
  CurTok = Driver->Lexx->lex(&CurSema, &CurLoc);
  LastTokWasNewlineTerminated = false;
  while (CurTok == NEWLINE) {
    LastTokWasNewlineTerminated = true;
    CurTok = Driver->Lexx->lex(&CurSema, &CurLoc);
  }
}
```

## The AST

The AST is heavily inspired by LLVM IR, although it has some higher-level
concepts like `Tensor`. It's an SSA and has a UseDef graph embedded in it,
making analysis and transformation easy.

The main classes are `Type` and `Value`. All types like `IntType`, `FloatType`
inherit from `Type`, most of the others inherit from `Value`. A `BasicBlock` is
a `Value`, and so is `ConstantInt`.

A `BasicBlock` is a vector of `Instruction`, and this is how the AST is an SSA:
assignments are handled as a `StoreInst`; there is no real LHS, just RHS
references.

```cpp
StoreInst::StoreInst(Value *MallocedValue, Value *NewValue);
```

## UseDef in AST

`Value` is uniquified using LLVM's `FoldingSet`, and `Use` wraps it, so we can
replace one `Value` with another.

```cpp
/// A Use is basically a linked list of Value wrappers
class Use {
  Value *Val;
  Use *Prev;
  Use *Next;
   // Laid out in memory as [User] - [Use1] - [Use2]. Use2 has DistToUser 2
  unsigned DistToUser;
};
```

An `Instruction` is a `User`. `User` and its `Use` values are laid out
sequentially in memory, so it's possible to reach all the `Use` values from the
`User`. It's also possible to reach the `User` from any `Use`, using
`DistToUser`.

```cpp
class User : public Value {
protected:
  unsigned NumOperands;
};
class Instruction : User;
```

The `User` has a custom `new` to allocate memory for the `Use` instances
as well.

```cpp
  void *User::operator new(size_t Size, unsigned Us) {
    void *Storage = ::operator new (Us * sizeof(Use) + Size);
    auto Start = static_cast<Use *>(Storage);
    auto End = Start + Us;
    for (unsigned Iter = 0; Iter < Us; Iter++) {
      new (Start + Iter) Use(Us - Iter);
    }
    auto Obj = reinterpret_cast<User *>(End);
    return Obj;
  }
};
```

## The Context

The Context is a somewhat large object that keeps the uniqified `Type` and
`Value` instances. It also keeps track of `Externals`, the external C functions
that are provided as part of a "standard library". Unique `llvm::Builder` and
`llvm::Context` objects, as well as the `DiagnosticPrinter` are exposed member
variables. Finally, it is necessary for symbol resolution, and keeps the
`ResolutionMap`.

## Symbol resolution

src/Transform/Resolve is an example of something that utilizes the UseDef embedded
in the AST.

```elixir
  B = A + 2
```

creates one `UnresolvedValue`, `A`, an `AddInst`, and a `MallocInst`,
which takes the string "B" and `AddInst` as operands.

The transform basically goes over all the `Instruction` in the `BasicBlock`,
resolves `UnresolvedValue` instances, and sets the `Use` to the resolved value.
It hence replaces the `Value` underneath the `Use`, and since the `Instruction`
is referencing `Use` instances, there are no dangling references.

```cpp
if (auto S = K->Map.get(V, Block)) {
  /// %S = 2;
  ///  ^
  /// Came from here (MallocInst, Argument, or Prototype)
  ///
  /// Foo(%S);
  ///      ^
  ///  UnresolvedValue; replace with %Replacement
  if (auto M = dyn_cast<MallocInst>(S)) {
    if (dyn_cast<StoreInst>(U->getUser()))
      U.set(M);
  }
}
```

## Type Inference

Type Inference is too simple. One `visit` function is overloaded for all
possible `Value` classes.

```cpp
Type *TypeInfer::visit(MallocInst *V) {
  V->setType(visit(V->getVal()));
  assert(!V->isUnTyped() && "unable to type infer MallocInst");
  return VoidType::get(K);
}
```

## Building

```sh
$ git submodule update --init
$ mkdir llvm-build
$ cd llvm-build
$ cmake -GNinja ../rhine/llvm
$ ninja
$ mkdir ../rhine-build
$ cd ../rhine-build
$ cmake -GNinja ../rhine
$ ninja
```

## Commentary

An inefficient untyped language is easy to implement. `println` taking 23 and
"twenty three" as arguments is a simple matter of switching on
type-when-unboxed. There's no need to rewrite the value in IR, and certainly no
need to come up with an overloading scheme.

[Crystal](http://crystal-lang.org/) made a good decision to start with Ruby. If
your idea is to self-host, then the original language's efficiency does not
matter. All you need is good generated assembly (which LLVM makes easy).
