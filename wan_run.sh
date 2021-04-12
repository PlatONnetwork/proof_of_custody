#!/bin/bash
set -x

# ====================
curdir=$(pwd)
builddir=${curdir}/build
install_dir=${curdir}/local
mkdir -p ${builddir}
# ====================

if [ $# -lt 1 ]; then
  echo "$0 <partyid> <parties:2> <hostname:localhost>"
  exit 1
fi

# run examples and tests
export LD_LIBRARY_PATH=${install_dir}/lib:$LD_LIBRARY_PATH
mkdir -p logs Player-Data

partyid=${1}
parties=${2:-2}
hostname=${3:-"localhost"}
portnum=${4:-29000}

prog=poc_main2fork.x
nohup ./${prog} -P ${partyid} -N ${parties} -h ${hostname} -pn ${portnum} -lgp1 381 -lgp2 256 >logs/${prog}-${partyid}.log 2>&1 &

echo "Start POC ${partyid}/${parties}."
