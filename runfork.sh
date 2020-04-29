#!/bin/bash
# set -x

curdir=$(pwd)
chmod +x *.sh

if [ $# -lt 1 ]; then
    echo "$0 <n>"
    exit 1
fi
n=$1

# compile
./compile.sh

# test
mkdir -p log out
for ((i = 1; i < $n; i++)); do
    ./mainfork $i >log/log$i.txt 2>&1 &
done
./mainfork 0

exit 0
