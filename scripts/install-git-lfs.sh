#!/bin/bash

if [ $TRAVIS_OS_NAME == linux ]; then
  wget https://github.com/github/git-lfs/releases/download/v1.1.2/git-lfs-linux-amd64-1.1.2.tar.gz &&
  tar xf git-lfs-linux-amd64-1.1.2.tar.gz &&
  export PATH=`pwd`/git-lfs-1.1.2:$PATH &&
  git checkout -f # old git errors on un-executable smudge/clean filters
fi
