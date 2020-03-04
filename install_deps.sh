#!/bin/bash

set -x

curdir=$(pwd)

builddir=/tmp/.poc/
mkdir -p ${builddir}
cp deps/* ${builddir}
cd ${builddir}

# Basic
sudo apt update
sudo apt install -y gcc g++ make automake build-essential
sudo apt install -y yasm libssl-dev expect

# GMP
# https://gmplib.org/
sudo apt install -y libgmp-dev

# MPIR
# http://www.mpir.org/downloads.html
cd ${builddir}
tar -xjf mpir-3.0.0.tar.bz2
cd mpir-3.0.0
./configure --enable-cxx
make -j8
sudo make install

# mcl
# https://github.com/herumi/mcl
cd ${builddir}
tar -zxf mcl-v1.05.tar.gz
cd mcl-1.05
make -j8
sudo make install
mkdir -p build
cd build
cmake ..
make -j8
sudo make install

#
sudo ldconfig
exit 0
