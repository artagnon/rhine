#!/bin/bash

if [ $TRAVIS_OS_NAME == linux ] || [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then
  # doesn't pick up the flex in $PATH
  cmake -GNinja -DFLEX_EXECUTABLE=$TOOLS_ROOT/flex ..
elif [ $TRAVIS_OS_NAME == osx ] || [ "$(uname)" == "Darwin" ]; then
  cmake -GNinja ..
fi
