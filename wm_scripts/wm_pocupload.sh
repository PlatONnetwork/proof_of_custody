#!/bin/bash
set -x

. ./wm_pocconfig.sh

for ((i = 0; i < $parties; i++)); do
  scp poc.sdk.tar.gz ${user}@${ips[i]}:${dstdir}
done
