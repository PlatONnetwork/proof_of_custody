#!/bin/bash
set -x
curdir=$(pwd)
third_dir=${curdir}/3rd
install_dir=${third_dir}/install
mkdir -p ${third_dir}
mkdir -p ${install_dir}

sudo apt install -y libboost-filesystem-dev clang-format-9
sudo pip install tatsu

# cryptopp
cd ${third_dir}
git clone -b CRYPTOPP_8_2_0 https://github.com/weidai11/cryptopp.git

cd ${third_dir}/cryptopp
CXX_FLAGS="-march=x86-64 -mtune=generic" make -j$(nproc) PREFIX=${install_dir}
make install PREFIX=${install_dir}

# relic
cd ${third_dir}
git clone https://github.com/relic-toolkit/relic.git

cd ${third_dir}/relic
git checkout 0998bfcb6b00aec85cf8d755d2a70d19ea3051fd

cd ${third_dir}/relic
mkdir -p build
cd build
cmake .. -DALLOC=AUTO -DWSIZE=64 -DWORD=64 -DRAND=UDEV -DSHLIB=ON -DSTLIB=ON \
    -DSTBIN=OFF -DTIMER=HREAL -DCHECK=on -DVERBS=on -DARITH=x64-asm-254 -DFP_PRIME=254 \
    -DFP_METHD="INTEG;INTEG;INTEG;MONTY;LOWER;SLIDE" \
    -DCOMP="-O3 -funroll-loops -fomit-frame-pointer -finline-small-functions -march=x86-64 -mtune=generic" \
    -DFP_PMERS=off -DFP_QNRES=on \
    -DFPX_METHD="INTEG;INTEG;LAZYR" -DPP_METHD="LAZYR;OATEP" \
    -DCMAKE_INSTALL_PREFIX=${install_dir}
make -j$(nproc)
make install

# HdrHistogram
cd ${third_dir}
git clone -b 0.9.12 https://github.com/HdrHistogram/HdrHistogram_c.git

cd ${third_dir}/HdrHistogram_c
mkdir build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=${install_dir} -DCMAKE_PREFIX_PATH=${install_dir}
make -j$(nproc)
make install

# googletest
cd ${third_dir}
git clone https://github.com/google/googletest.git

cd ${third_dir}/googletest
git checkout e93da23920e5b6887d6a6a291c3a59f83f5b579e

cd ${third_dir}/googletest
mkdir build
cd build
cmake .. -DCMAKE_CXX_FLAGS="-std=c++11 -march=x86-64 -mtune=generic" \
    -DCMAKE_INSTALL_PREFIX=${install_dir} -DCMAKE_PREFIX_PATH=${install_dir}
make -j$(nproc)
make install

# concord-bft
cd ${third_dir}
git clone -b v0.9.0 https://github.com/vmware/concord-bft.git

# cd ${third_dir}/concord-bft
# git checkout -b v0.7

mkdir -p build
cd build
cmake .. -DBUILD_TESTING=ON -DUSE_CONAN=OFF -DUSE_OPENTRACING=OFF \
    -DCMAKE_INSTALL_PREFIX=${install_dir} -DCMAKE_PREFIX_PATH=${install_dir} \
    -DHDR_HISTOGRAM_INCLUDE_DIR=${install_dir}/include -DCMAKE_MODULE_PATH=${curdir}/cmake

#  -DBUILD_COMM_TCP_PLAIN=ON
#  -DCMAKE_MODULE_PATH=${curdir}/cmake
make -j$(nproc)
make install

cd ${curdir}
exit 0
