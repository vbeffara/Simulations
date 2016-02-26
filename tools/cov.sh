#!/usr/bin/env sh -e

cmake -G Ninja -DCOV=1 build
ninja -C build

mkdir -p build/cov

lcov -z -d build
./build/tests/tests
lcov -c -d build -d libvb -o build/cov/cov.info --gcov-tool gcov-5 --no-external -q

genhtml -q -o build/cov build/cov/cov.info

# open build/cov/index.html

cmake -G Ninja -DCOV=0 build
ninja -C build
