#!/bin/bash
set -x

. ./wm_pocconfig.sh

for ((i = 0; i < $parties; i++)); do
  sleep 0.2
  ssh ${user}@${ips[i]} "cd ${dstdir}/.sdk; ./kill.sh"
done
