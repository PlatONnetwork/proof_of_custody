#!/bin/bash
# set -x

curdir=$(pwd)
chmod +x *.sh

for ((i = 0; i < 3; i++)); do
    for ((n = 3; n < 21; n += 2)); do
        echo "runing i:${i} n:${n} ..."
        ./vm_run_single.sh $n
        sleep 2
    done
done
