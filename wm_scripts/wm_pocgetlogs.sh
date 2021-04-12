#!/bin/bash
set -x

. ./wm_pocconfig.sh

mkdir -p poclogs
for ((i = 0; i < $parties; i++)); do
  ssh ${user}@${ips[i]} "cd ${dstdir}/.sdk/; tar -zcf logs-$parties-$i.tar.gz logs"
  sleep 0.2
  scp ${user}@${ips[i]}:${dstdir}/.sdk/logs-$parties-$i.tar.gz ./poclogs/
done
