#!/bin/bash

curdir=$(pwd)
chmod +x *.sh

killall -q main.x

# compile
mkdir -p ${curdir}/build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j
cd ..

bindir=${curdir}/build/bin
ln -sf ${bindir}/Setup.x Setup.x
ln -sf ${bindir}/main.x main.x
ln -sf ${bindir}/mainfork mainfork
ln -sf ${bindir}/mainfork-sbft mainfork-sbft

exit 0
