# Rhine: An optionally-typed Elixir

Rhine is designed to be a fast language utilizing the LLVM JIT featuring N-d
tensors, first-class functions, optional typing.

Rhine started off as [rhine-ml](https://github.com/artagnon/rhine-ml).

Effort put into rhine-ml: 2 months
Effort put into Rhine: 1 year, 5 months

## Language Features

```elixir
def fptrTest do
  return println
end
def mathTest(Input ~Int, Finput ~Float) do
  if true do
    println $ 3 + Input
  else
    Lam = fn Arg -> Arg * 2 end
    fptrTest (Lam 4)
  end
  ret 4
end
```

rhine-ml has arrays, first-class functions, closures, variadic arguments,
macros. Rhine only has the first two.

## Why C++

- Speed of execution
- Curiosity to see how hard it would be
- The LLVM API is first-class in C++
- Intent of exercising the part of the LLVM API not accessible from the OCaml
  bindings

## Differences with rhine-ml

Rhine is typed while rhine-ml is not. As a result, Rhine does full type
inference, and rhine-ml gets away with boxing-unboxing.

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

The Rhine build is decoupled from the LLVM build because you would want
fine-grained control in this case. LLVM is not to be accidentally rebuilt: the
build is quite expensive.


## Lessons learnt

- Writing a compiler in C++ is not to be done by one person. It requires a lot
  more manpower.
