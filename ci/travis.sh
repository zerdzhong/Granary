#!/usr/bin/env sh

mkdir build || true
cd build
cmake ..
make
CTEST_OUTPUT_ON_FAILURE=1 make test
cd test
ctest --verbose
