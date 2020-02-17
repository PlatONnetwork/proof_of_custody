#!/bin/bash

killall main.x

mkdir -p build
cd build
cmake ..
make -j8
cd ..

ln -sf ./build/main.x main.x

mkdir -p log
./main.x 2 >log/log2 2>&1 &
./main.x 1 >log/log1 2>&1 &
./main.x 0

exit 0
