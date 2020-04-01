#!/bin/bash

curdir=$(pwd)

if [ $# -lt 2 ]; then
    echo "$0 <n> <i>"
    exit 1
fi
n=$1
i=$2
((t = ($n - 1) / 2))

rm -rf Data Cert-Store
cp -r ./xpublish/Data-$n-$t Data
cp -r ./xpublish/Cert-Store-$n-$t Cert-Store

mkdir -p log out

export LD_LIBRARY_PATH=./lib
logname=$(date "+%Y%m%d%H%M%S")
logname=log-${n}-${t}-${i}-${logname}.txt

nohup ./main.x $i >log/${logname} 2>&1 &

exit 0
