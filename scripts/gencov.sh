#!/bin/bash

cd ~/src/rhine/rhine-build &&
lcov --capture --directory . -o rawcoverage.info &&
lcov --remove rawcoverage.info "/Applications/*" "/usr/*" "gtest/*" \
"llvm/*" "rhine-build/*" -o coverage.info &&
rm -rf cov &&
genhtml coverage.info -o cov
