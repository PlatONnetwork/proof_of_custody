#!/bin/bash

set -x

curdir=$(pwd)
chmod +x *.sh

# compile
./compile.sh

rm -rf vm/xpublish
mkdir -p vm/xpublish

rm -rf vm/xdata
mkdir -p vm/xdata
python ./vm_gen_networkdata.py

function xf() {
    n=$1
    t=$2
    echo $n $t

    ./gen_certs.sh ${n}
    if [ $? -ne 0 ]; then
        exit 1
    fi

    mkdir -p Data
    cp vm/xdata/NetworkData-$n-$t.txt Data/NetworkData.txt

    ./gen_sharing.sh ${t}

    mv Data vm/xpublish/Data-$n-$t
    mv Cert-Store vm/xpublish/Cert-Store-$n-$t
}

for ((n = 3; n < 19; n+=2)); do
    ((t = ($n - 1) / 2))
    xf $n $t
done
xf 20 9

mkdir -p vm
cp -f main.x vm/
