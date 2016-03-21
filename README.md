# rhine: A typed Elixir-inspired language on LLVM

rhine is designed to be a fast language utilizing the LLVM JIT featuring N-d
tensors, first-class functions, and type inference; specifying argument
types is enough. It has a full blown AST into which it embeds a UseDef graph.

rhine started off as [rhine-ml](https://github.com/artagnon/rhine-ml), and
rhine-ml was called rhine earlier.

- Effort put into rhine-ml: 2 months
- Effort put into rhine: 1 year, 1 month

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

`~Int` is a type annotation, and only argument types need to be annotated,
return type is inferred. `~Function(Int -> Int -> Int)` is a function that takes
two integers and returns one integer, mixing in some Haskell syntax. `$` is
again from Haskell, which is basically like putting the RHS in parens.

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

The desired directory structure is:
```
bin/ ; if you downloaded the tarball for this
    cmake
    ninja
    flex
src/
    rhine/
            README.md
            llvm/ ; git submodule update --init to get the sources
            llvm-build/
                        bin/
                            llvm-config ; you need to call this to build
    rhine-build/
            rhine ; the executable
```

On an OSX where you have everything:

```sh
$ git submodule update --init
$ mkdir llvm-build
$ cd llvm-build
# rhine is buggy; without debugging symbols, you can't report a useful bug
$ cmake -GNinja -DCMAKE_BUILD_TYPE=Debug ../llvm
# this will install to /usr/local
$ ninja install
$ mkdir ../rhine-build
$ cd ../rhine-build
$ cmake -GNinja ../rhine
# this will run the packages unittests, which should all pass
$ ninja check
```

On a Linux where you have nothing (and no root privileges are required):

First, untar cmake-ninja-flex.tar.xz and set up environment variables.

```sh
$ tar xf cmake-ninja-flex.tar.xz
$ cd cmake-ninja-flex

# for bash/zsh
$ export TOOLS_ROOT=`pwd`
$ export PATH=$TOOLS_ROOT:$PATH
# for csh
$ setenv TOOLS_ROOT `pwd`
$ setenv PATH $TOOLS_ROOT:$PATH
```

Then,

```sh
$ git submodule update --init
$ mkdir llvm-build
$ cd llvm-build
# rhine is buggy; without debugging symbols, you can't report a useful bug
$ cmake -GNinja -DCMAKE_BUILD_TYPE=Debug ../llvm
$ ninja
```

Add a symlink to `bin/llvm-config` in `$TOOLS_ROOT`.

```sh
$ mkdir ../rhine-build
$ cd ../rhine-build
# flex isn't picked up from $PATH
$ cmake -GNinja -DTOOLS_ROOT=$TOOLS_ROOT -DFLEX_EXECUTABLE=$TOOLS_ROOT/flex ../rhine
# if there are build (usually link) errors, please open an issue
$ ninja check
```

## Commentary

An inefficient untyped language is easy to implement. `println` taking 23 and
"twenty three" as arguments is a simple matter of switching on
type-when-unboxed. There's no need to rewrite the value in IR, and certainly no
need to come up with an overloading scheme.

[Crystal](http://crystal-lang.org/) made a good decision to start with Ruby. If
your idea is to self-host, then the original language's efficiency does not
matter. All you need is good generated assembly (which LLVM makes easy).
