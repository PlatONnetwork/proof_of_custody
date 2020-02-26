#!/bin/bash
# set -x

curdir=$(pwd)

if [ $# -lt 1 ]; then
    echo "$0 <n>"
    exit 1
fi
n=$1

mkdir -p Data
NDFile=./Data/NetworkData.txt
echo "RootCA.crt" >${NDFile}

echo "${n}" >>${NDFile}

for ((i = 0; i < $n; i++)); do
    id=$i
    id2=$(expr $i + 1)
    echo "${id} 127.0.0.1 Player${id2}.crt P${id2}" >>${NDFile}
done

echo "0" >>${NDFile}
echo "0" >>${NDFile}

exit 0
