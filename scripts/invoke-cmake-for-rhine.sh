#!/bin/bash

if [ $TRAVIS_OS_NAME == linux ]; then
  # doesn't pick up the flex in $PATH
  cmake -GNinja -DFLEX_EXECUTABLE=$TOOLS_ROOT/flex ..
fi
if [ $TRAVIS_OS_NAME == osx ]; then
  cmake -GNinja ..
fi
