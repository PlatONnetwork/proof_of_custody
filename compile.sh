#!/bin/bash

curdir=$(pwd)
chmod +x *.sh

killall -q main.x

mkdir -p build
cd build
cmake ..
make -j8
cd ..

bindir=./build/bin
ln -sf ${bindir}/Setup.x Setup.x
ln -sf ${bindir}/main.x main.x
ln -sf ${bindir}/mainfork mainfork
ln -sf ${bindir}/mainfork-sbft mainfork-sbft

exit 0
