# Rhine

Rhine is designed to be a fast language utilizing the LLVM JIT
featuring first-class functions, optional typing supported by a
combination of type inference and speculative type inference, and a
strong FFI.

## Building

```
$ git clone --recursive https://github.com/artagnon/rhine
$ mkdir llvm-build
$ cd llvm-build
$ cmake ../rhine/llvm
$ make -j8
$ mkdir ../rhine-build
$ cd ../rhine-build
$ cmake ../rhine
$ make -j8
```

## Syntax

```
def fptrTest do
  return println
end
def mathTest(Input, Output) do
  if true do
    println $ 3 + 2
  else
    fptrTest 4
  end
  ret 4
end
```