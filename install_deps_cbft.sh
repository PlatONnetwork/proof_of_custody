#!/bin/bash
set -x
home=$(pwd)
third_dir=${home}/3rd
mkdir -p ${third_dir}

cd ${third_dir}
git clone https://github.com/weidai11/cryptopp.git -b CRYPTOPP_8_2_0

cd ${third_dir}/cryptopp && make -j4 && make install
# make PREFIX=.cryptopp_install install

#cd ${third_dir}
#git clone https://github.com/relic-toolkit/relic.git

#cd ${third_dir}/relic
#git checkout b984e901ba78c83ea4093ea96addd13628c8c2d0

#mkdir -p build && cd build && cmake -DWSIZE=64 -DRAND=UDEV -DSHLIB=ON -DSTBIN=ON -DTIMER=HREAL -DCHECK=on -DVERBS=on -DARITH=x64-asm-254 -DFP_PRIME=254 -DFP_METHD="INTEG;INTEG;INTEG;MONTY;LOWER;SLIDE" -DCOMP="-O3 -funroll-loops -fomit-frame-pointer -finline-small-functions -march=native -mtune=native" -DFP_PMERS=off -DFP_QNRES=on -DFPX_METHD="INTEG;INTEG;LAZYR" -DPP_METHD="LAZYR;OATEP" ..
#-DCMAKE_INSTALL_PREFIX=.relic_install &&
#make -j4 && make install

cd ${third_dir}
git clone https://github.com/vmware/concord-bft.git

cd ${third_dir}/concord-bft
mkdir -p build && cd build && cmake -DBUILD_TESTING=ON -DUSE_CONAN=OFF -DUSE_OPENTRACING=OFF \
    -DBUILD_COMM_TCP_PLAIN=ON -DCMAKE_MODULE_PATH=${home}/cmake .. && make -j4
