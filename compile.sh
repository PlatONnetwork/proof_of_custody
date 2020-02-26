#!/bin/bash

curdir=$(pwd)
chmod +x *.sh

killall main.x

mkdir -p build
cd build
cmake ..
make -j8
cd ..

ln -sf ./build/main.x main.x
ln -sf ./build/Setup.x Setup.x

exit 0
