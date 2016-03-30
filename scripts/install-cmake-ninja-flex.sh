#!/bin/bash

if [ $TRAVIS_OS_NAME == linux ]; then
  file cmake-ninja-flex.tar.bz2 &&
  pwd &&
  tar xf cmake-ninja-flex.tar.bz2 &&
  export TOOLS_ROOT=`pwd`/cmake-ninja-flex &&
  export PATH=$TOOLS_ROOT:$PATH
fi

if [ $TRAVIS_OS_NAME == osx ]; then
  brew update &&
  brew install flex ninja &&
  brew link --force flex
fi
