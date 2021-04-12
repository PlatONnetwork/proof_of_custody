#!/bin/bash
set -x

. ./wm_pocconfig.sh

for ((i = 0; i < $parties; i++)); do
  sleep 0.2
  nohup ssh ${user}@${ips[i]} "cd ${dstdir}/.sdk/; ./wan_run.sh $i $parties $hostname $portnum" &
done
