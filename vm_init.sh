#!/bin/bash

curdir=$(pwd)
chmod +x *.sh

rm -rf vm/xpublish
mkdir -p vm/xpublish

function xf() {
    n=$1
    t=$2
    echo $n $t

    ./init.sh ${n} ${t}

    mv Data vm/xpublish/Data-$n-$t
    mv Cert-Store vm/xpublish/Cert-Store-$n-$t
}

for ((n = 3; n < 21; n++)); do
    ((t = ($n - 1) / 2))
    xf $n $t
done

mkdir -p vm
cp -f main.x vm/
