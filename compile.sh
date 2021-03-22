#!/bin/bash

curdir=$(pwd)
builddir=${curdir}/build
install_dir=${curdir}/local
mkdir -p ${builddir}
mkdir -p logs

# spdz
cd ${builddir}
cmake .. -DCMAKE_INSTALL_PREFIX=${install_dir} -DCMAKE_PREFIX_PATH=${install_dir}
make -j8

# run examples
export LD_LIBRARY_PATH=${install_dir}/lib:$LD_LIBRARY_PATH

cd ${curdir}
# ./compile.py tutorial
# Scripts/mascot.sh tutorial
# cp ${builddir}/mascot-party.x ./ -f
# ./mascot-party.x 1 tutorial -pn 27021 -h localhost -N 2 >logs/tutorial-1 2>&1 &
# ./mascot-party.x 0 tutorial -pn 27021 -h localhost -N 2 | tee logs/tutorial-0 2>&1

# #2
Scripts/setup-ssl.sh 2
cp ${builddir}/mascot-example.x ./ -f
./mascot-example.x 1 2 >logs/mascot-example2-1 2>&1 &
./mascot-example.x 0 2 | tee logs/mascot-example2-0 2>&1

# #3
# sleep 3
# Scripts/setup-ssl.sh 3
# ./mascot-example.x 2 3 >logs/mascot-example3-2 2>&1 &
# ./mascot-example.x 1 3 >logs/mascot-example3-1 2>&1 &
# ./mascot-example.x 0 3 | tee logs/mascot-example3-0 2>&1
