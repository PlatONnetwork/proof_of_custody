#!/bin/bash

killall Player.x

mkdir -p build
cd build
cmake ..
make -j8
cd ..

ln -sf ./build/Player.x Player.x

./Player.x 2 >log2 2>&1 &
./Player.x 1 >log1 2>&1 &
./Player.x 0

exit 0

./Player.x 2 Programs/0add >log2 2>&1 &
./Player.x 1 Programs/0add >log1 2>&1 &
./Player.x 0 Programs/0add
