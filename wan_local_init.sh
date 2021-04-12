#!/bin/bash
set -x

# ====================
curdir=$(pwd)
builddir=${curdir}/build
install_dir=${curdir}/local
mkdir -p ${builddir}
# ====================

mkdir -p logs Player-Data

parties=${1:-2}
Scripts/setup-ssl.sh $parties
