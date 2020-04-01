#!/bin/bash

curdir=$(pwd)

if [ $# -lt 1 ]; then
    echo "$0 <n>"
    exit 1
fi
n=$1

for ((i = 0; i < $n; i++)); do
    echo "simulator runing ${i}/${n} ..."
    ./vm_run_wan.sh $n $i
    sleep 2
done
