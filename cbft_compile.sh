#!/bin/bash
set -x

# ====================
curdir=$(pwd)
builddir=${curdir}/build
install_dir=${curdir}/local
mkdir -p ${builddir}
# ====================

build_type=Debug
build_type=Release
verbose=OFF
verbose=ON

# cbft
mkdir -p ${builddir}/cbft
cd ${builddir}/cbft
cmake ../../cbft/ -DCMAKE_INSTALL_PREFIX=${install_dir} -DCMAKE_PREFIX_PATH=${install_dir} \
  -DBUILD_TESTING=ON -DUSE_CONAN=OFF -DUSE_OPENTRACING=OFF
make -j8
# -DBUILD_COMM_TCP_PLAIN=ON
# BUILD_COMM_TCP_TLS