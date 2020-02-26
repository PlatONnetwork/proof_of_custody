#!/bin/bash
set -x

killall main.x

mkdir -p build
cd build
cmake ..
make -j8
cd ..

ln -sf ./build/main.x main.x

echo $#

n=3
if [ $# -ge 1 ]; then
    n=$1
fi

# check n if valid

mkdir -p log
for ((i = 1; i < $n; i++)); do
    ./main.x $i >log/log$i.txt 2>&1 &
done
./main.x 0 >log/log0.txt 2>&1
#./main.x 0

exit 0
