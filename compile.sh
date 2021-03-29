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
verbose=ON
verbose=OFF

# poc
cd ${builddir}
cmake .. -DCMAKE_INSTALL_PREFIX=${install_dir} -DCMAKE_PREFIX_PATH=${install_dir} \
  -DCMAKE_BUILD_TYPE=${build_type} -DVERBOSE=${verbose}
make -j8
