#!/bin/bash
# set -x

curdir=$(pwd)

if [ $# -lt 1 ]; then
    echo "$0 <n>"
    exit 1
fi
n=$1

mkdir -p Cert-Store
cd Cert-Store

# Init random
openssl rand -writerand ~/.rnd

# Root CA
openssl genrsa -out RootCA.key 4096
openssl req -new -x509 -days 1826 -key RootCA.key -out RootCA.crt \
    -subj "/C=UK/ST=Bristol/L=Bristol/O=Root CA/CN=Root CA"

# Players
for ((i = 0; i < $n; i++)); do
    id=$(expr $i + 1)
    openssl genrsa -out Player${id}.key 2048
    openssl req -new -key Player${id}.key -out Player${id}.csr \
        -subj "/C=UK/ST=Bristol/L=Bristol/O=Player ${id}/CN=P${id}"
    openssl x509 -req -days 1000 -in Player${id}.csr -CA RootCA.crt -CAkey RootCA.key -set_serial 0101 -out Player${id}.crt -sha256
done

rm -rf *.csr
cd ${curdir}

# View
#openssl req -noout -text -in XXXX.csr
#openssl x509 -noout -text -in XXXX.crt

exit 0
