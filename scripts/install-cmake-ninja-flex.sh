#!/bin/bash

if [ $TRAVIS_OS_NAME == linux ] || [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then
  tar xf cmake-ninja-flex.tar.bz2 &&
  export TOOLS_ROOT=`pwd`/cmake-ninja-flex &&
  export PATH=$TOOLS_ROOT:$PATH
elif [ $TRAVIS_OS_NAME == osx ] || [ "$(uname)" == "Darwin" ]; then
  brew update &&
  brew install cmake ninja flex &&
  brew link --force flex
fi
