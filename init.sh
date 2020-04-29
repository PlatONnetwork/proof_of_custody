#!/bin/bash

curdir=$(pwd)
chmod +x *.sh

if [ $# -lt 2 ]; then
    echo "$0 <n> <t>"
    exit 1
fi
n=$1
t=$2

# compile
./compile.sh

#
./gen_certs.sh ${n}
if [ $? -ne 0 ]; then
    exit 1
fi
./gen_networkdata.sh ${n} ${t}
if [ $? -ne 0 ]; then
    exit 1
fi

./gen_sharing1.sh ${t}
cp Data/SharingData.txt Data/SharingData1.txt

./gen_sharing.sh ${t}

exit 0
