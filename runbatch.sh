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
function run_once() {
    for ((i = 1; i < $n; i++)); do
        ./main.x $i >log/log$i.txt 2>&1 &
    done
    ./main.x 0 >log/log0.txt 2>&1
    #./main.x 0
}
for ((j = 0; j < 50; j++)); do
    run_once
    echo $j
    sleep 1
done
