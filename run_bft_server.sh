#!/bin/bash
set -x
curdir=$(pwd)
bindir=${curdir}/build/bin
mkdir -p ${bindir}

cd ${bindir}

../../3rd/concord-bft/build/tools/GenerateConcordKeys -n 4 -f 1 -o private_file_

./server 0 &
./server 1 &
./server 2 &
./server 3
