#!/bin/bash

curdir=$(pwd)

if [ $# -lt 1 ]; then
    echo "$0 <n>"
    exit 1
fi
n=$1

((t = ($n - 1) / 2))

rm -rf Data Cert-Store
cp -r ./xpublish/Data-$n-$t Data
cp -r ./xpublish/Cert-Store-$n-$t Cert-Store

mkdir -p log out

export LD_LIBRARY_PATH=./lib
logname=$(date "+%Y%m%d%H%M%S")
logname=log-${n}-${t}-${i}-${logname}.txt

for ((i = 1; i < $n; i++)); do
    ./main.x $i >log/${logname} 2>&1 &
done
./main.x 0 >log/${logname} 2>&1
#./main.x 0

cd ${curdir}
echo "run" $n/$t "done!"
