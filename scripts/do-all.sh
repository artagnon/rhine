#!/bin/sh

./scripts/install-git-lfs.sh && # linux-only
./scripts/install-cmake-ninja-flex.sh && # .tar.bz2 on linux, brew on mac
git submodule update --init &&
cd llvm-build &&
cmake -GNinja -DCMAKE_BUILD_TYPE=Debug ../llvm &&
ninja &&
export PATH=`pwd`/bin:$PATH &&
cd ../rhine-build &&
./scripts/invoke-cmake-for-rhine.sh && # -DFLEX_EXECUTABLE= extra on linux
ninja check
