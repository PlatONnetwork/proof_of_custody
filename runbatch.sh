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
    ./scripts/run_program.sh main.x $n
}
for ((j = 0; j < 50; j++)); do
    echo -e "\n$j ----------------------"
    time run_once
    sleep 1
done
