#!/bin/bash
# set -x

bash ./pre_install.sh

# ====================
curdir=$(pwd)
builddir=${curdir}/build
install_dir=${curdir}/local
mkdir -p ${builddir}
# ====================

# mcl
# https://github.com/herumi/mcl
cd ${curdir}/mcl
make -j8
make install PREFIX=${install_dir}
mkdir -p build
cd build
CXXFLAGS="-Wl,-rpath -Wl,./local/lib -L./local/lib" cmake .. -DCMAKE_INSTALL_PREFIX=${install_dir} -DCMAKE_PREFIX_PATH=${install_dir}
make -j8
make install
cd ${curdir}

# mpir
cd ${curdir}/mpir
./configure --enable-cxx --prefix=${install_dir}
autoreconf -i
make -j8
make install
cd ${curdir}

# SimpleOT
cd ${curdir}/SimpleOT
make -j8
cd ${curdir}
